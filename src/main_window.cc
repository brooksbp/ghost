// Copyright 2014 Ghost Authors. All rights reserved.
// Use of this source code is governed by a ALv2 license that can be
// found in the LICENSE file.

#include "main_window.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QHeaderView>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
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
  table_model_ = new TableModel(0, library_);

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
}

void MainWindow::Init(Library* library) {
  table_model_->beginReset();
  library_ = library;
  table_model_->endReset();
}

void MainWindow::PlaybackProgress(uint64_t pos, uint64_t len) {
  if (slider_engaged_)
    return;

  // Normalize playback position
  while (pos >= 1000000) { pos /= 1000; }
  while (len >= 1000000) { len /= 1000; }
  uint64_t r = (pos * 1000) / len;
  
  if (r <= 1000) {
    qDebug() << "setting slider value " << r;
    slider_->setValue(r);
  }
}

void MainWindow::handleButtonPressed() {
  
}

void MainWindow::handleSliderPressed() {
  slider_engaged_ = true;
  qDebug() << "slider engaged";
}
void MainWindow::handleSliderMoved(int value) {
  qDebug() << "slide moved";
}
void MainWindow::handleSliderReleased() {
  slider_engaged_ = false;
  qDebug() << "slider disengaged";
}


void MainWindow::handleDoubleClick(const QModelIndex& index) {
  library_->Play(index.row());
}


TableModel::TableModel(QObject* parent, Library* library)
    : QAbstractTableModel(parent),
      library_(library) {
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
  return library_->GetNumTracks();
}

int TableModel::columnCount(const QModelIndex&) const {
  return 1;
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
  if (role == Qt::DisplayRole) {
    Track* track = library_->GetTrack(index.row());
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
  QModelIndex tl = createIndex(0, 0);
  QModelIndex br = createIndex(library_->GetNumTracks() - 1, 0);
  emit dataChanged(tl, br);
}

