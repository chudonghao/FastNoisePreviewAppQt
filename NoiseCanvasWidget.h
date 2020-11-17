//
// Created by chudonghao on 2020/11/17.
//

#ifndef FASTNOISEPREVIEWAPPQT_9E81F22F3B8A42419B245548BE21F4F9_H_
#define FASTNOISEPREVIEWAPPQT_9E81F22F3B8A42419B245548BE21F4F9_H_

#include <queue>

#include <QImage>
#include <QWidget>
#include <QTime>

#include <FastNoiseLite.h>

class QThread;

class NoiseCanvasWidget_CreateQueue;
class NoiseCanvasWidget_CreateQueue_CreateResult;

class NoiseCanvasWidget : public QWidget {
  Q_OBJECT
public:
  enum PreviewDimension {
    PREVIEW_DIMENSION_2D,
    PREVIEW_DIMENSION_3D,
  };
  struct PreviewConfig {
    FastNoiseLite noise;
    QSize size;
    PreviewDimension dimension;
    float z{};
    /// delta z for scroll
    /// \see FastNoiseLite::mFrequency is private
    float dz{1.f};
  };

public:
  using Self = NoiseCanvasWidget;
  using Super = QWidget;

  explicit NoiseCanvasWidget(QWidget *parent);
  ~NoiseCanvasWidget() override;

  void preview(PreviewConfig config);
  int getTime4Create() const;
  float getMean() const;
  float getMax() const;
  float getMin() const;
  Q_SIGNAL void previewChanged();
  Q_SLOT void setScroll(bool);

protected:
  Q_SLOT void on_CreateQueue_NewNoiseImageCreated(const NoiseCanvasWidget_CreateQueue_CreateResult&);
private:
  // void customEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

private:
  QThread *_createThread{};
  NoiseCanvasWidget_CreateQueue *_queue{};
  NoiseCanvasWidget_CreateQueue_CreateResult *_lastResult{};
  PreviewConfig _lastPreviewConfig;
  QTime _lastPreviewCallTime;
  bool _scroll{false};
};

struct NoiseCanvasWidget_CreateQueue_CreateResult {
  QImage image;
  int time; // ms
  float mean;
  float max;
  float min;
};

class NoiseCanvasWidget_CreateQueue : public QObject {
  Q_OBJECT
public:

  NoiseCanvasWidget_CreateQueue();
  Q_SIGNAL void newNoiseImageCreated(NoiseCanvasWidget_CreateQueue_CreateResult);
private:
  struct QueueItem {};

private:
  void customEvent(QEvent *event) override;
  void create();
  std::queue<NoiseCanvasWidget::PreviewConfig> _queue;
};

Q_DECLARE_METATYPE(NoiseCanvasWidget_CreateQueue_CreateResult)

#endif // FASTNOISEPREVIEWAPPQT_9E81F22F3B8A42419B245548BE21F4F9_H_
