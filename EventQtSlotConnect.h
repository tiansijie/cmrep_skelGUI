#ifndef EventQtSlotConnect_H
#define EventQtSlotConnect_H

#include "ui_EventQtSlotConnect.h"
#include "VoronoiSkeletonTool.h"
#include "AddTagDialog.h"
#include "MouseInteractorAdd.h"
//#include "constants.h"

#include <QMainWindow>
#include <vtkSmartPointer.h>
#include <QFutureWatcher>
#include <QtGui>
#include <vtkPolyData.h>

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
	QComboBox* getTagComboBox();
	void readCustomData(vtkPolyData *polydata);
	void readCustomDataTri(vtkDoubleArray* triDBL);
	void readCustomDataEdge(vtkDoubleArray* edgeDBL);
	void readCustomDataPoints(vtkDoubleArray* ptsDBL);
	void readCustomDataTag(vtkDoubleArray* tagDBL);
	void readCustomDataLabel(vtkDoubleArray* labelDBL);

	//vtkSmartPointer<MouseInteractorAdd> style;
	//MouseInteractorAdd kk;
	//MouseInteractorAdd *style;
	

public slots:
	void slot_clicked(vtkObject*, unsigned long, void*, void*);
	void slot_position(double x, double y, double z);
	void slot_finished();
	void slot_skelStateChange(int);
	void slot_addTag();
	void slot_comboxChanged(int);

	void slot_open();
	void slot_save();
	
	void executeCmrepVskel();	
private:

	vtkSmartPointer<vtkEventQtSlotConnect> Connections;
	QFutureWatcher<void> FutureWatcher;
	VoronoiSkeletonTool v;

	QMenu *fileMenu;
	QAction *openAct;
	QAction *saveAct;

	std::string VTKfilename;  
	vtkPolyData* polyObject;
};

#endif
