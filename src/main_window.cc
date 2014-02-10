#include "main_window.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QHeaderView>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  central_ = new QWidget(this);
  central_layout_ = new QGridLayout(central_);
  setCentralWidget(central_);

  // Slider
  slider_ = new QSlider(Qt::Horizontal, 0);
  
  central_layout_->addWidget(slider_);

  // TableView
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


  resize(800, 400);
}

void MainWindow::Initialize(Library* library) {
    library_ = library;
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
      return QString(track->file_path_.value().c_str());
    }
  }
  return QVariant();
}
