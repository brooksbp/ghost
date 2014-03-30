// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "ui/main_window.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QHeaderView>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  Library::GetInstance()->AddObserver(this);
  GstPlayer::GetInstance()->AddObserver(this);
  LOG(INFO) << "MainWindow()";
}

void MainWindow::Shutdown() {
  Library::GetInstance()->RemoveObserver(this);
  GstPlayer::GetInstance()->RemoveObserver(this);

  delete table_model_;
  delete table_view_;
  delete slider_;
  delete play_button_;
  delete hbox_;
  delete central_;
}

void MainWindow::closeEvent(QCloseEvent* event) {
  event->ignore();

  extern void InitiateShutdown(void);
  InitiateShutdown();
}

void MainWindow::Init() {
  // +-------------------------------------------+
  // |:central_layout_                           |
  // |+-----------------------------------------+|
  // ||:hbox_    buttons, slider                ||
  // ||                                         ||
  // |+-----------------------------------------+|
  // |     tracks_table                          |
  // +-------------------------------------------+

  central_ = new QWidget(this);
  central_layout_ = new QGridLayout(central_);
  setCentralWidget(central_);

  hbox_ = new QHBoxLayout;
  central_layout_->addLayout(hbox_, 0, 0);

  // Play control buttons ------------------------------------------------------
  play_button_ = new QPushButton;
  play_button_->setText("--");

  connect(
      play_button_, SIGNAL(released()), this, SLOT(handleButtonPressed()));
  
  hbox_->addWidget(play_button_);

  // Slider --------------------------------------------------------------------
  slider_ = new QSlider(Qt::Horizontal, 0);
  slider_->setMaximum(1000);
  slider_->setMinimum(0);
  slider_engaged_ = false;

  connect(slider_, SIGNAL(sliderPressed()), this, SLOT(handleSliderPressed()));
  connect(slider_, SIGNAL(sliderMoved(int)), this, SLOT(handleSliderMoved(int)));
  connect(slider_, SIGNAL(sliderReleased()), this, SLOT(handleSliderReleased()));
  
  hbox_->addWidget(slider_);

  // Tracks table --------------------------------------------------------------
  table_view_ = new QTableView();
  table_model_ = new TableModel(this);
  //LOG(INFO) << "zzz";

  table_view_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_view_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table_view_->verticalHeader()->setVisible(false);

  table_view_->setModel(table_model_);

  connect(table_view_, SIGNAL(doubleClicked(const QModelIndex&)),
          this, SLOT(handleDoubleClick(const QModelIndex&)));
  
  table_view_->show();

  central_layout_->addWidget(table_view_);

  //----------------------------------------------------------------------------
  resize(800, 400);
  LOG(INFO) << "MainWindow::Init()";
}

void MainWindow::OnEndOfStream() {
  play_button_->setText("--");
  slider_->setValue(0);
}

void MainWindow::OnPositionUpdated(float position) {
  if (slider_engaged_)
    return;

  // Ignore the first .5s worth of updates due to jitter.
  if (position < 0.5f)
    return;

  track_position_ = position;

  float percent_complete = (track_position_ / track_duration_) * 100;
  slider_->setValue(percent_complete * 10);
}

void MainWindow::OnDurationUpdated(float duration) {
  track_duration_ = duration;
  LOG(INFO) << "duration = " << duration;
}

void MainWindow::handleButtonPressed() {
  if (GstPlayer::GetInstance()->IsPlaying()) {
    GstPlayer::GetInstance()->Pause();
    play_button_->setText("play");
  } else {
    GstPlayer::GetInstance()->Play();
    play_button_->setText("pause");
  }
}

void MainWindow::handleSliderPressed() {
  slider_engaged_ = true;
}
void MainWindow::handleSliderMoved(int value) {
  float val = ((float) value) / 10;
  float time = (val * track_duration_) / 100;
  GstPlayer::GetInstance()->Seek(time);
}
void MainWindow::handleSliderReleased() {
  slider_engaged_ = false;
}


void MainWindow::handleDoubleClick(const QModelIndex& index) {
  Track* track = Library::GetInstance()->GetTrack(index.row());
  if (!track)
    return;

#if defined(OS_WIN)
  GstPlayer::GetInstance()->Load(base::WideToUTF8(track->file_path_.value()));
#elif defined(OS_POSIX)
  GstPlayer::GetInstance()->Load(track->file_path_.value());
#endif
  GstPlayer::GetInstance()->Play();
  
  play_button_->setText("pause");
}


TableModel::TableModel(QObject* parent)
    : QAbstractTableModel(parent) {
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
      case 0:
        return tr("Track");
      case 1:
        return tr("??");
      default:
        return QVariant();
    }
  }
  return QVariant();
}

int TableModel::rowCount(const QModelIndex&) const {
  return Library::GetInstance()->GetNumTracks();
}

int TableModel::columnCount(const QModelIndex&) const {
  return 1;
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole) {
    Track* track = Library::GetInstance()->GetTrack(index.row());
    if (track) {
#if defined(OS_WIN)
      return QString::fromWCharArray(track->file_path_.value().c_str());
#else
      return QString(track->file_path_.value().c_str());
#endif
    }
  }
  return QVariant();
}

void TableModel::emitDataChanged() {
  int num_tracks = Library::GetInstance()->GetNumTracks();
  QModelIndex tl = createIndex(0, 0);
  QModelIndex br = createIndex((num_tracks > 0) ? num_tracks - 1 : 0, 0);
  emit dataChanged(tl, br);
}

void MainWindow::OnBeginImport(Library* library) {
  table_model_->beginReset();
}
void MainWindow::OnFinishImport(Library* library) {
  table_model_->endReset();
}
