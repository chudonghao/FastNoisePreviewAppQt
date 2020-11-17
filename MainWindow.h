//
// Created by chudonghao on 2020/11/17.
//

#ifndef FASTNOISEPREVIEWAPPQT_4B315A382DD5403C810DE64B3440580A_H_
#define FASTNOISEPREVIEWAPPQT_4B315A382DD5403C810DE64B3440580A_H_

#include <QMainWindow>

namespace Ui {
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  using Self = MainWindow;
  using Super = QMainWindow;

public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  Q_SLOT void on_GenerateButton_Clicked();
  Q_SLOT void on_NoiseCanvas_PreviewChanged();

private:
  using Ui = ::Ui::MainWindow;

private:
  void generate();

  Ui *ui;
};

#endif // FASTNOISEPREVIEWAPPQT_4B315A382DD5403C810DE64B3440580A_H_