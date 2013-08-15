#ifndef MOUSERINTERACTORADD_H
#define MOUSERINTERACTORADD_H


#include <vtkGenericDataObjectReader.h>

#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkProperty.h>

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkRendererCollection.h>
#include <vtkCellArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPointPicker.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkPolyLine.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGeometry.h>
#include <vtkAreaPicker.h>

#include <vtkInteractorStyleSwitch.h>
#include <vtkDoubleArray.h>
#include <vtkAppendPolyData.h>

#include <vtkDelaunay3D.h>
#include <vtkDelaunay2D.h>
#include <vtkShrinkFilter.h>
#include <vtkTriangle.h>
#include <vtkPointData.h>
#include <vtkPolygon.h>
#include <vtkPolyDataReader.h>
#include <vtkGenericDataObjectWriter.h>
#include <sstream>
#include <vtkPolyDataNormals.h>
#include <vtkFloatArray.h>
#include <vtkMath.h>
#include <vtkLineSource.h>
#include <vtkArrowSource.h>

#include <vtkAffineWidget.h>
#include <vtkAffineRepresentation2D.h>
#include <vtkCommand.h>
#include <vtkTransform.h>
#include <vtkObject.h>
#include <vtkDecimatePro.h>
#include <vtkTriangleFilter.h>
#include <vtkQuadricDecimation.h>
#include <vtkPointPicker.h>
#include <vtkCellPicker.h>

#include <QtGui>
#include <QObject>

#include "global.h"

//To record different actions
#define VIEW 0
#define ADDPOINT 1
#define DELETEPOINT 2
#define CREATETRI 3
#define DELETETRI 4
#define FLIPNORMAL 5
#define CHANGETRILABEL 6
#define PICKPTTRI 7
#define DESELECTPT 8
#define MOVEPT 9

#define SHOW 0
#define HIDE 1

using namespace std;


// Handle mouse events
class MouseInteractor : public QObject, public vtkInteractorStyleTrackballCamera
{
	Q_OBJECT
public:
	static MouseInteractor* New();
	vtkTypeMacro(MouseInteractor, vtkInteractorStyleTrackballCamera);

	MouseInteractor();

	double Distance(double p1[3], double p2[3]);
	int ConstrainEdge(int type1, int type2);
	int PairNumber(int a, int b);
	int DrawTriangle();
	bool DrawTriangle(TagTriangle);
	TagTriangle DeleteTriangle(double*);
	bool DeleteTriangle(TagTriangle);
	vtkActor* PickActorFromPoints(double pos[3]);
	vtkActor* PickActorFromTriangle(double pos[3]);
	bool FlipNormal(double*);
	TagPoint AddPoint(double*);
	bool AddPoint(TagPoint);
	TagPoint DeletePoint(double*);
	bool DeletePoint(TagPoint tagpt);
	int PickPointForTri(double*);
	void CheckNormal(int triId[3]);
	void AddDecimateEdge(int pointSeq);
	int ChangeTriLabel(double pos[3]);
	bool isValidEdge(int id1, int id2);
	void SetNextTriPt();
	void SetNextTriPtHelper(int id1, int id2);
	bool MovePoint(double pos[3]);
	void MovePoint(int ptIndex, int oldSeq);
	bool MoveTriangle(double pos[3], int triIndex, int num, int newSeq);
	void SelectMovePt(double pos[3]);

	void copyEdgeBtoA(int a, int b);
	int deleteEdgeHelper(int id1, int id2, int seq);
	int deleteEdgeHelper2(int id, int seq);
	void deleteEdge(int seq);
	void setNormalGenerator(vtkSmartPointer<vtkPolyDataNormals> normalGenerator);
	void updateLabelTriNum();
	void updateLabelPtNum();
	void reset();

	void DrawDelaunayTriangle();
	void AutoTriangulation();
	
	virtual void OnLeftButtonDown();
	virtual void OnMiddleButtonDown();
	void OnKeyRelease();
	void OnKeyPress();	

	std::vector<int>deletePointIds;
	std::vector<int>triPtIds;

	vtkActor *selectedTriangle;
	vtkActor *prePolyLineActor;

	//Display the number points and triangles
	QLabel *labelPtNumber;
	QLabel *labelTriNumber;

	int operationFlag;
	int preOperationFlag;
	std::string preKey;
	int skelState;
	int meshState;
	int currentTriIndex;

	int actionCounter;
	QColor triLabelColors[10];


	//Using reference to store all the global value
	std::vector<TagInfo> &vectorTagInfo;
	std::vector<TagTriangle> &vectorTagTriangles;
	std::vector<TagPoint> &vectorTagPoints;
	std::vector<TagEdge> &vectorTagEdges;

	//Store all the label info, 0 represent no tag on this point
	std::vector<double> &labelData;
	std::vector<vtkActor*> &triNormalActors;
	bool &isSkeleton;
	int &selectedTag;
	double &targetReduction;
	double &featureAngle;
	double &tagRadius;
	
	double backCol[3];

	bool &decimateMode;


//Several signal to emit the state changed
signals:
	void operationChanged(int);
	void skelStateChanged(int);
	void meshStateChanged(int);

private:
	

	void DoAction(int action);
	//Flip normal and change triangle label;
	void DoAction(int action, double pos[3], int triIndex = -1);
	//For point interaction(add point, delete point)
	void DoAction(int action, TagPoint pointInfo, int ptIndex);
	//For triangle interaction(draw triangle, delete triangle)
	void DoAction(int action, TagTriangle triangleInfo);
	//
	void DoAction(int action, int ptIndex);
	//For moving point interaction
	void DoAction(int action, int ptIndex, int ptOldSeq);

	void UndoAction();

	bool drawTriMode;
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator;
	
	//Store each operation, use for undo function
	std::vector<TagAction> vectorActions;		

	bool isCtrlPress;
	//Record moving point index
	int movePtIndex;
};
#endif
