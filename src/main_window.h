#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSlider>

#include <QtCore/QDebug>
#include <QtCore/QAbstractTableModel>

#include "library.h"
#include "base/basictypes.h"

namespace Ui {

class MainWindow;

}

class TableModel;


class MainWindow : public QMainWindow {
  Q_OBJECT
 public:
  explicit MainWindow(QWidget* parent = 0);

  void Init(Library* library);

  void PlaybackProgress(uint64_t pos, uint64_t len);
                                   
 private slots:
  void handleButtonPressed();

  void handleSliderPressed();
  void handleSliderMoved(int value);
  void handleSliderReleased();

  void handleDoubleClick(const QModelIndex& index);
  
 private:
  QWidget* central_;
  QGridLayout* central_layout_;
  QHBoxLayout* hbox_;

  QPushButton* play_button_;

  QSlider* slider_;
  bool slider_engaged_;
  
  QTableView* table_view_;
  TableModel* table_model_;

  Library* library_;
};


class TableModel : public QAbstractTableModel {
  Q_OBJECT
 public:
  TableModel(QObject* parent = 0, Library* library = 0);

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  int rowCount(const QModelIndex& parent) const;
  int columnCount(const QModelIndex& parent) const;
  QVariant data(const QModelIndex& index, int role) const;
  
  void beginReset() { beginResetModel(); }
  void endReset() { endResetModel(); }

  // Use when structure of indices don't change, otherwise use xxxReset().
  void emitDataChanged();

 private:
  Library* library_;
};

#endif  // MAIN_WINDOW_H_
