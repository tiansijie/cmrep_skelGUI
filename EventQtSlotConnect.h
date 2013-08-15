#ifndef EventQtSlotConnect_H
#define EventQtSlotConnect_H

#include "ui_EventQtSlotConnect.h"
#include "VoronoiSkeletonTool.h"
#include "AddTagDialog.h"
#include "MouseInteractor.h"

#include <QMainWindow>
#include <vtkSmartPointer.h>
#include <QFutureWatcher>
#include <QtGui>
#include <vtkPolyData.h>
#include <vtkStringArray.h>

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
	~EventQtSlotConnect();
	void createActions();
	void createMenus();
	void readVTK(std::string filename);
	QComboBox* getTagComboBox();
	void readCustomData(vtkPolyData *polydata);
	void readCustomDataTri(vtkFloatArray* triDBL);
	void readCustomDataEdge(vtkFloatArray* edgeDBL);
	void readCustomDataPoints(vtkFloatArray* ptsDBL);
	void readCustomDataTag(vtkFloatArray* tagDBL, vtkStringArray* tagStr);
	void readCustomDataLabel(vtkFloatArray* labelDBL);

	void saveVTKFile(QString fileName);
	void saveParaViewFile(QString fileName);
	void saveCmrepFile(QString fileName);

	void Decimate();

public slots:
	void slot_finished();
	void slot_skelStateChange(int);
	void slot_meshStateChange(int);
	void slot_addTag();
	void slot_delTag();
	void slot_editTag();
	void slot_comboxChanged(int);

	void slot_gridTypeChanged(int);
	void slot_solverTypeChanged(int);
	void slot_consRadiusCheck(int);
	
	void slot_open();
	void slot_save();

	void slot_targetReductSilder(int);
	void slot_targetReductEditor(QString);
	void slot_featureAngleSlider(int);
	void slot_feartureAngleEditor(QString);
	void slot_decimateButton();

	void slot_tagSizeSlider(int);

	void slot_addPoint();
	void slot_deletePoint();
	void slot_createTri();
	void slot_deleteTri();
	void slot_flipNormal();
	void slot_view();
	void slot_changeTriLabel();
	void slot_movePoint();

	void slot_updateOperation(int);

	void slot_updateProgressBar();

	void slot_skelTransparentChanged(int);

	void slot_trilabelChanged(int);

	void executeCmrepVskel();	

private:

	void loadSettings();
	void saveSettings();
	void setToolButton(int flag);
	void iniTriLabel();

	QString settingsFile;

	vtkSmartPointer<vtkEventQtSlotConnect> Connections;
	QFutureWatcher<void> FutureWatcher;
	VoronoiSkeletonTool v;

	QMenu *fileMenu;
	QAction *openAct;
	QAction *saveAct;

	std::string VTKfilename;  
	vtkPolyData* polyObject;

	double targetReduction;
	double featureAngle;

	vtkSmartPointer<MouseInteractor> mouseInteractor;

	int progressSignalCount;

	QColor triLabelColors[10];
};

#endif
