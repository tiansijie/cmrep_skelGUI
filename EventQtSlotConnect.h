#ifndef EventQtSlotConnect_H
#define EventQtSlotConnect_H

#include "ui_EventQtSlotConnect.h"


#include <QMainWindow>
#include <vtkSmartPointer.h>
#include <QFutureWatcher>
#include <QtGui>

#include "VoronoiSkeletonTool.h"


QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class vtkEventQtSlotConnect;

class EventQtSlotConnect : public QMainWindow, private Ui::EventQtSlotConnect
{
  Q_OBJECT
public:

  EventQtSlotConnect();

  void createActions();

  void createMenus();

  void readVTK(std::string filename);

public slots:
  void slot_clicked(vtkObject*, unsigned long, void*, void*);
  void slot_position(double x, double y, double z);
  void open();
  void save();
  void executeCmrepVskel();
  void slot_finished();

private:

  vtkSmartPointer<vtkEventQtSlotConnect> Connections;
  QFutureWatcher<void> FutureWatcher;
  VoronoiSkeletonTool v;

  QMenu *fileMenu;
  QAction *openAct;
  QAction *saveAct;

  std::string VTKfilename;  
};

#endif
