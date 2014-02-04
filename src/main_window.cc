#include "main_window.h"

#include <QtCore/QCoreApplication>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  button_ = new QPushButton("the button", this);
  button_->setGeometry(QRect(QPoint(100,100), QSize(200,50)));
  connect(button_, SIGNAL(released()), this, SLOT(HandleButton()));
}

void MainWindow::HandleButton() {
  audio_manager_->TogglePause();
}
