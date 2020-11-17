#include <iostream>

#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QApplication::setOrganizationName("chudonghao");
  QApplication::setApplicationName("FastNoisePreviewAppQt");
  QApplication::setApplicationDisplayName("FastNoisePreviewAppQt");

  MainWindow mw;
  mw.show();

  return QApplication::exec();
}
