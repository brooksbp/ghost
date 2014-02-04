#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>

#include "audio_manager.h"

namespace Ui {

class MainWindow;

}

class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget* parent = 0);

  void SetAudioManager(AudioManager* am) { audio_manager_ = am; }

 private slots:
  void HandleButton();

 private:
  QPushButton* button_;

  AudioManager* audio_manager_;
};

#endif  // MAIN_WINDOW_H_
