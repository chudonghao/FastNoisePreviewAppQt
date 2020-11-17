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

  FastNoiseLite &noise = _queue.front().noise;
  QSize &size = _queue.front().size;
  auto dimension = _queue.front().dimension;
  float z = _queue.front().z;

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
      float n = 0.f;
      if (dimension == decltype(dimension)::PREVIEW_DIMENSION_2D) {
        n = noise.GetNoise<float>(column, row);
      } else {
        n = noise.GetNoise<float>(column, row, z);
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

  int time = startTime.msecsTo(QTime::currentTime());

  QImage img(size, QImage::Format::Format_RGB888);
  for (int column = 0; column < size.width(); ++column) {
    for (int row = 0; row < size.height(); ++row) {
      QVector3D _min = QVector3D(min, min, min);
      QVector3D _max = QVector3D(max, max, max);
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

  _queue.pop();
}

static int registerMetaType4CreateResult() { return qRegisterMetaType<NoiseCanvasWidget_CreateQueue_CreateResult>("NoiseCanvasWidget_CreateQueue_CreateResult"); }

NoiseCanvasWidget_CreateQueue::NoiseCanvasWidget_CreateQueue() { static int _register = registerMetaType4CreateResult(); }
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
    int deffer = QTime::currentTime().msecsTo(_lastPreviewCallTime.addMSecs(200));
    QTimer::singleShot(std::max(deffer, 0), this, [=] {
      if (_scroll) {
        _lastPreviewConfig.z += _lastPreviewConfig.dz;
        preview(_lastPreviewConfig);
      }
    });
  }
}
