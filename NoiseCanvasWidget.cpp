//
// Created by chudonghao on 2020/11/17.
//

#include "NoiseCanvasWidget.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <QVector3D>

#include <FastNoiseLite.h>

namespace {

struct NewCreateRequestEvent : public QEvent {
  NewCreateRequestEvent(const NoiseCanvasWidget::PreviewConfig &config) : QEvent(QEvent::User), config(config) {}
  NoiseCanvasWidget::PreviewConfig config;
};

struct ExecCreateEvent : public QEvent {
  ExecCreateEvent() : QEvent(QEvent::User) {}
};

} // namespace

//////////////////////////////////
/// NoiseCanvasWidget_CreateQueue

static int registerMetaType4CreateResult() { return qRegisterMetaType<NoiseCanvasWidget_CreateQueue_CreateResult>("NoiseCanvasWidget_CreateQueue_CreateResult"); }

NoiseCanvasWidget_CreateQueue::NoiseCanvasWidget_CreateQueue() { static int _register = registerMetaType4CreateResult(); }

void NoiseCanvasWidget_CreateQueue::customEvent(QEvent *event) {
  auto ncr = dynamic_cast<NewCreateRequestEvent *>(event);
  if (ncr) {
    _queue.push(ncr->config);
    QApplication::postEvent(this, new ExecCreateEvent);
    return;
  }
  create();
}

void NoiseCanvasWidget_CreateQueue::create() {
  while (_queue.size() > 1) {
    _queue.pop();
  }
  if (_queue.empty()) {
    return;
  }
  if (!_queue.front().visualiseDomainWarp) {
    createNoiseImage();
  } else {
    createDomainWarpImage();
  }
}

void NoiseCanvasWidget_CreateQueue::createNoiseImage() {
  FastNoiseLite &noise = _queue.front().noise;
  QSize &size = _queue.front().size;
  auto invert = _queue.front().invert;
  auto dimension = _queue.front().dimension;
  float Z = _queue.front().z;

  if (size.isEmpty()) {
    _queue.pop();
    return;
  }

  std::vector<std::vector<QVector3D>> noiseMat;
  noiseMat.resize(size.width(), std::vector<QVector3D>(size.height(), QVector3D{0.f, 0.f, 0.f}));

  QTime startTime = QTime::currentTime();

  float mean = 0.f;
  float max = std::numeric_limits<float>::lowest();
  float min = std::numeric_limits<float>::max();
  float sum = 0.f;
  int count = 0;
  for (int column = 0; column < size.width(); ++column) {
    for (int row = 0; row < size.height(); ++row) {
      float x = column;
      float y = row;
      float z = Z;

      switch (noise.GetDomainWarpType()) {
        case FastNoiseLite::DomainWarpType_OpenSimplex2:
        case FastNoiseLite::DomainWarpType_OpenSimplex2Reduced:
        case FastNoiseLite::DomainWarpType_BasicGrid:
          if (dimension == decltype(dimension)::PREVIEW_DIMENSION_2D) {
            noise.DomainWarp(x, y);
          } else {
            noise.DomainWarp(x, y, z);
          }
          break;
      }

      float n = 0.f;
      if (dimension == decltype(dimension)::PREVIEW_DIMENSION_2D) {
        n = noise.GetNoise<float>(x, y);
      } else {
        n = noise.GetNoise<float>(x, y, z);
      }
      if (invert) {
        n = -n;
      }

      noiseMat[column][row].setX(n);
      noiseMat[column][row].setY(n);
      noiseMat[column][row].setZ(n);

      max = std::max(n, max);
      min = std::min(n, min);
      sum += n;
      ++count;
    }
  }
  mean = sum / count;

  createImage(size, noiseMat, mean, max, min, startTime.msecsTo(QTime::currentTime()));

  _queue.pop();
}

void NoiseCanvasWidget_CreateQueue::createDomainWarpImage() {
  FastNoiseLite &noise = _queue.front().noise;
  QSize &size = _queue.front().size;
  auto invert = _queue.front().invert;
  auto dimension = _queue.front().dimension;
  float Z = _queue.front().z;

  if (size.isEmpty()) {
    _queue.pop();
    return;
  }

  std::vector<std::vector<QVector3D>> noiseMat;
  noiseMat.resize(size.width(), std::vector<QVector3D>(size.height(), QVector3D{0.f, 0.f, 0.f}));

  QTime startTime = QTime::currentTime();

  float mean = 0.f;
  float max = std::numeric_limits<float>::lowest();
  float min = std::numeric_limits<float>::max();
  float sum = 0.f;
  int count = 0;
  for (int column = 0; column < size.width(); ++column) {
    for (int row = 0; row < size.height(); ++row) {
      float x = column;
      float y = row;
      float z = Z;

      switch (noise.GetDomainWarpType()) {
        case FastNoiseLite::DomainWarpType_OpenSimplex2:
        case FastNoiseLite::DomainWarpType_OpenSimplex2Reduced:
        case FastNoiseLite::DomainWarpType_BasicGrid:
          if (dimension == decltype(dimension)::PREVIEW_DIMENSION_2D) {
            noise.DomainWarp(x, y);
          } else {
            noise.DomainWarp(x, y, z);
          }
          break;
      }

      float wx = x - column;
      float wy = y - row;
      float wz = z - Z;
      noiseMat[column][row].setX(wx);
      noiseMat[column][row].setY(wy);
      noiseMat[column][row].setZ(wz);

      max = std::max(std::max({wx, wy, wz}), max);
      min = std::min(std::min({wx, wy, wz}), min);
      sum += wx + wy + wz;
      count += 3;
    }
  }
  mean = sum / count;

  createImage(size, noiseMat, mean, max, min, startTime.msecsTo(QTime::currentTime()));

  _queue.pop();
}

void NoiseCanvasWidget_CreateQueue::createImage(const QSize &size, const std::vector<std::vector<QVector3D>> &noiseMat, float mean, float max, float min, int time) {
  if (min == max) {
    // avoid 0
    max = min + 1;
  }
  QVector3D _min = QVector3D(min, min, min);
  QVector3D _max = QVector3D(max, max, max);
  QImage img(size, QImage::Format_RGB888);
  for (int column = 0; column < size.width(); ++column) {
    for (int row = 0; row < size.height(); ++row) {
      QVector3D v = noiseMat[column][row];
      v = (v - _min) / (_max - _min) * 255;
      img.setPixelColor(column, row, QColor(v.x(), v.y(), v.z()));
    }
  }

  NoiseCanvasWidget_CreateQueue_CreateResult result;
  result.image = img;
  result.time = time;
  result.mean = mean;
  result.max = max;
  result.min = min;
  emit newNoiseImageCreated(result);
}

//////////////////////
/// NoiseCanvasWidget

NoiseCanvasWidget::NoiseCanvasWidget(QWidget *parent) : QWidget(parent) {
  _lastResult = new NoiseCanvasWidget_CreateQueue_CreateResult;

  _createThread = new QThread(this);
  _createThread->start();
  _queue = new NoiseCanvasWidget_CreateQueue();
  _queue->moveToThread(_createThread);

  connect(_queue, &NoiseCanvasWidget_CreateQueue::newNoiseImageCreated, this, &Self::on_CreateQueue_NewNoiseImageCreated);
}

NoiseCanvasWidget::~NoiseCanvasWidget() {
  _createThread->quit();
  _createThread->wait();
  delete _lastResult;
}

void NoiseCanvasWidget::preview(PreviewConfig config) {
  _lastPreviewCallTime = QTime::currentTime();
  _lastPreviewConfig = config;
  QApplication::postEvent(_queue, new NewCreateRequestEvent(config));
}

const QImage &NoiseCanvasWidget::getCurrentImage() const { return _lastResult->image; }

int NoiseCanvasWidget::getTime4Create() const { return _lastResult->time; }

float NoiseCanvasWidget::getMean() const { return _lastResult->mean; }

float NoiseCanvasWidget::getMax() const { return _lastResult->max; }

float NoiseCanvasWidget::getMin() const { return _lastResult->min; }

void NoiseCanvasWidget::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.drawImage(rect(), _lastResult->image, rect());
}

void NoiseCanvasWidget::setScroll(bool scroll) {
  _scroll = scroll;
  if (_scroll) {
    preview(_lastPreviewConfig);
  }
}

void NoiseCanvasWidget::on_CreateQueue_NewNoiseImageCreated(const NoiseCanvasWidget_CreateQueue_CreateResult &result) {
  *_lastResult = result;
  update();
  emit previewChanged();

  //////////
  // scroll

  if (_scroll) {
    float T = (float) _lastPreviewCallTime.msecsTo(QTime::currentTime()) / 1000;
    scrollUp(T);
  }
}

void NoiseCanvasWidget::scrollUp(float n) {
  _lastPreviewConfig.z += 1.f / _lastPreviewConfig.noise.GetFrequency() * n;
  preview(_lastPreviewConfig);
}

void NoiseCanvasWidget::scrollDown(float n) {
  _lastPreviewConfig.z -= 1.f / _lastPreviewConfig.noise.GetFrequency() * n;
  preview(_lastPreviewConfig);
}
