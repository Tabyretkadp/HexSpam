#include "./main/include/mainwindow.h"
#include "./main/include/tdapp.h"

#include <QApplication>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;

  w.setStyleSheet("background-color: #191919");
  w.setFixedSize(1200, 800);

  w.show();
  w.setStackIndex(1);

  return a.exec();
}
