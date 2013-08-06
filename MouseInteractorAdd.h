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

#include <QtGui>
#include <QObject>

#include "constants.h"

#define VIEW 0
#define ADDPOINT 1
#define DELETEPOINT 2
#define CREATETRI 3
#define DELETETRI 4
#define FLIPNORMAL 5
#define CHANGETRILABEL 6
#define PICKPTTRI 7

#define SHOW 0
#define HIDE 1


// Handle mouse events
class MouseInteractorAdd : public QObject, public vtkInteractorStyleTrackballCamera
{
	Q_OBJECT
public:
	static MouseInteractorAdd* New();
	vtkTypeMacro(MouseInteractorAdd, vtkInteractorStyleTrackballCamera);

	MouseInteractorAdd();

	double Distance(double p1[3], double p2[3]);
	void SetSelectedTriColor();
	int ConstrainEdge(int type1, int type2);
	int PairNumber(int a, int b);
	int DrawTriangle();
	bool DrawTriangle(TagTriangle);
	TagTriangle DeleteTriangle(double*);
	bool DeleteTriangle(TagTriangle);
	vtkActor* PickActorFromMesh(double pos[3]);
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

	void copyEdgeBtoA(int a, int b);
	int deleteEdgeHelper(int id1, int id2, int seq);
	int deleteEdgeHelper2(int id, int seq);
	void deleteEdge(int seq);
	void setNormalGenerator(vtkSmartPointer<vtkPolyDataNormals> normalGenerator);
	void setLabelTriNum();
	void setLabelPtNum();
	void reset();

	void DrawDelaunayTriangle();
	void AutoTriangulation();
	
	virtual void OnLeftButtonDown();
	void OnKeyRelease();
	void OnKeyPress();

	

	std::vector<int>deletePointIds;
	std::vector<int>triPtIds;

	vtkActor *selectedTriangle;
	vtkActor *prePolyLineActor;

	QLabel *labelPtNumber;
	QLabel *labelTriNumber;

	int operationFlag;
	int preOperationFlag;
	std::string preKey;
	int skelState;
	int meshState;
	int currentTriIndex;


	static std::vector<TagInfo> vectorTagInfo;
	static std::vector<TagTriangle> vectorTagTriangles;
	static std::vector<TagPoint> vectorTagPoints;
	static std::vector<std::vector<TagPoint> > vectorClassifyPoints;
	static std::vector<TagEdge> vectorTagEdges;
	//store all the label info, 0 represent no tag on this point
	static std::vector<double> labelData;
	static std::vector<vtkActor*> triNormalActors;
	static bool isSkeleton;
	static int selectedTag;
	static double targetReduction;
	static double featureAngle;
	static double tagRadius;

	static double triCol[3];
	static double backCol[3];

	static bool decimateMode;

	int actionCounter;
	QColor triLabelColors[10];
signals:
	void operationChanged(int);
	void skelStateChanged(int);
	void meshStateChanged(int);

private:
	void DoAction(int action);
	void DoAction(int action, double pos[3], int triIndex = -1);//flip normal and change triangle label;
	void DoAction(int action, TagPoint pointInfo);//for point interaction
	void DoAction(int action, TagTriangle triangleInfo);//for triangle interaction
	void UndoAction();
	void RedoAction();	

	bool drawTriMode;
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator;
	std::vector<TagAction> vectorActions;		
	bool isCtrlPress;
};
#endif
