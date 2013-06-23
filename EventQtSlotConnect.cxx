#include "EventQtSlotConnect.h"
#include "AddTagDialog.h"


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


double pointColor[3];


struct TagInfo
{
	std::string tagName;
	int tagType; // 1 = Branch point  2 = Free Edge point 3 = Interior point  4 = others
	int tagColor[3];//for color
	QColor qc;//for color	
};
std::vector<TagInfo> vectorTagInfo;

//store all the label info, 0 represent no tag on this point
std::vector<double> labelData;

bool isSkeleton;

struct TagTriangle{
	vtkActor *triActor;
	double p1[3], p2[3], p3[3];
	double centerPos[3];
	int id1,id2,id3;
};
std::vector<TagTriangle>vectorTagTriangles;

struct TagPoint{
	vtkActor* actor;
	std::string typeName;
	int typeIndex;
	double radius;//radius of that points
	int seq;//the sequence in all points
	double pos[3];
};	
std::vector<TagPoint> vectorTagPoints;
std::vector<std::vector<TagPoint>> vectorClassifyPoints;

struct TagEdge
{
	int ptId1;
	int ptId2;
	int constrain;
	int numEdge;
	int seq;//use of deletion
};
std::vector<TagEdge> vectorTagEdges;

// Handle mouse events
class MouseInteractorAdd : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorAdd* New();
	vtkTypeMacro(MouseInteractorAdd, vtkInteractorStyleTrackballCamera);

	MouseInteractorAdd(){
		selectedTriangle = NULL;
		qtObject = NULL;
		triCol[0] = 0.2; triCol[1] = 0.2; triCol[2] = 0.7;
	}

	EventQtSlotConnect *qtObject;
	
	std::vector<int>deletePointIds;
	std::vector<int>triPtIds;
	vtkActor *selectedTriangle;
	vtkActor *prePolyLineActor;

	double Distance(double p1[3], double p2[3]){
		return std::sqrt(std::pow(p1[0] - p2[0],2) + std::pow(p1[1] - p2[1], 2) + std::pow(p1[2] - p2[2], 2));
	}

	void SetSelectedTriColor(){
		for(int i = 0; i < vectorTagTriangles.size(); i++){
			if(selectedTriangle == vectorTagTriangles[i].triActor)
				selectedTriangle->GetProperty()->SetColor(1,1,0);
			else
				vectorTagTriangles[i].triActor->GetProperty()->SetColor(triCol);
		}
	}

	void DrawDelaunayTriangle(){
		if(vectorTagPoints.size() > 3){
			vtkSmartPointer<vtkPoints> pts =
				vtkSmartPointer<vtkPoints>::New();
			for(int i = 0; i < vectorTagPoints.size(); i++){
				pts->InsertNextPoint(vectorTagPoints[i].pos);
			}

			// Create a polydata to store everything in
			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			polyData->SetPoints(pts);


			// Create a cell array to store the polygon in
			vtkSmartPointer<vtkCellArray> aCellArray =
				vtkSmartPointer<vtkCellArray>::New();

			// Define a polygonal hole with a clockwise polygon
			vtkSmartPointer<vtkPolygon> aPolygon =
				vtkSmartPointer<vtkPolygon>::New();

			//aPolygon->GetPointIds()->InsertNextId(1);
			//aPolygon->GetPointIds()->SetNumberOfIds(3);
			if(deletePointIds.size()!=0){
				aPolygon->GetPointIds()->InsertNextId(deletePointIds[2]);
				aPolygon->GetPointIds()->InsertNextId(deletePointIds[1]);
				aPolygon->GetPointIds()->InsertNextId(deletePointIds[0]);
			}
			deletePointIds.clear();
			//double n[3];
			//aPolygon->ComputeNormal(3, static_cast<double*>(aPolygon->GetPoints()->GetData()->GetVoidPointer(0)), n);
			aCellArray->InsertNextCell(aPolygon);

			vtkSmartPointer<vtkPolyData> boundary =
				vtkSmartPointer<vtkPolyData>::New();
			boundary->SetPoints(polyData->GetPoints());
			boundary->SetPolys(aCellArray);

			/*vtkSmartPointer<vtkPoints> pts2 =
				vtkSmartPointer<vtkPoints>::New();
			if(deletePointIds.size()!=0){
				pts2->InsertNextPoint(vectorTagPoints[deletePointIds[2]].p);
				pts2->InsertNextPoint(vectorTagPoints[deletePointIds[1]].p);
				pts2->InsertNextPoint(vectorTagPoints[deletePointIds[0]].p);
			}

			vtkSmartPointer<vtkPolyData> polyData2 = vtkSmartPointer<vtkPolyData>::New();
			polyData2->SetPoints(pts2);*/

			vtkSmartPointer<vtkDelaunay2D> delaunay2 =
				vtkSmartPointer<vtkDelaunay2D>::New();

			delaunay2->SetInput(boundary);
			delaunay2->Update();

			vtkSmartPointer<vtkPolyDataMapper> meshMapper2 =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			meshMapper2->SetInputConnection(delaunay2->GetOutputPort());

			vtkSmartPointer<vtkActor> meshActor2 =
				vtkSmartPointer<vtkActor>::New();
			meshActor2->SetMapper(meshMapper2);
			meshActor2->GetProperty()->SetColor(0.1,0.2,0.9);
			//this->GetDefaultRenderer()->AddActor(meshActor2);

			vtkSmartPointer<vtkDelaunay2D> delaunay =
				vtkSmartPointer<vtkDelaunay2D>::New();
			#if VTK_MAJOR_VERSION <= 5
			delaunay->SetInput(polyData);
			delaunay->SetSource(delaunay2->GetOutput());
			//delaunay->SetSourceConnection(delaunay2->GetOutputPort());
			//delaunay->SetSourceConnection()
			#else
			delaunay->SetInputData(polyData);
			delaunay->SetSourceData(boundary);
			#endif
			delaunay->Update();

			//delaunay->SetAlpha(20);
			//delaunay->SetTolerance(10);
			//delaunay->SetProjectionPlaneMode(2);
			// Visualize
			/*vtkSmartPointer<vtkShrinkFilter> shrink = 
			vtkSmartPointer<vtkShrinkFilter>::New();
			shrink->SetInputConnection(delaunay->GetOutputPort());
			shrink->SetShrinkFactor(0.9);*/

			/*vtkSmartPointer<vtkDataSetMapper> meshMapper =
				vtkSmartPointer<vtkDataSetMapper>::New();*/
			vtkSmartPointer<vtkPolyDataMapper> meshMapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			meshMapper->SetInputConnection(delaunay->GetOutputPort());

			vtkSmartPointer<vtkActor> meshActor =
				vtkSmartPointer<vtkActor>::New();
			meshActor->SetMapper(meshMapper);
			meshActor->GetProperty()->SetColor(0.2,0.8,0.5);
			meshActor->GetProperty()->SetRepresentationToWireframe();

			/*vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter =
			vtkSmartPointer<vtkVertexGlyphFilter>::New();
			#if VTK_MAJOR_VERSION <= 5
			glyphFilter->SetInputConnection(polyData->GetProducerPort());
			#else
			glyphFilter->SetInputData(polyData);
			#endif
			glyphFilter->Update();

			vtkSmartPointer<vtkPolyDataMapper> pointMapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
			pointMapper->SetInputConnection(glyphFilter->GetOutputPort());*/

/*
			// Add the points to the dataset
			polyData->SetPoints(pts);

			// Add the lines to the dataset
			polyData->SetLines(cells);

			// Setup actor and mapper
			vtkSmartPointer<vtkPolyDataMapper> mapper = 
				vtkSmartPointer<vtkPolyDataMapper>::New();
	#if VTK_MAJOR_VERSION <= 5
			mapper->SetInput(polyData);
	#else
			mapper->SetInputData(polyData);
	#endif*/
			/*vtkSmartPointer<vtkActor> actor = 
				vtkSmartPointer<vtkActor>::New();
			if(prePolyLineActor != NULL)
				this->GetDefaultRenderer()->RemoveActor(prePolyLineActor);
			prePolyLineActor = meshActor;*/
			//actor->SetMapper(pointMapper);
			//actor->GetProperty()->SetColor(0,1,1);
			//this->GetDefaultRenderer()->AddActor(actor);
			if(prePolyLineActor != NULL)
				this->GetDefaultRenderer()->RemoveActor(prePolyLineActor);
			prePolyLineActor = meshActor;
			this->GetDefaultRenderer()->AddActor(meshActor);
		}
		else{
			if(prePolyLineActor != NULL)
				this->GetDefaultRenderer()->RemoveActor(prePolyLineActor);
		}
	}

	void DrawRegularTriangle(){
		if(vectorTagPoints.size() > 2){
			double lastPos[3];
			lastPos[0] = vectorTagPoints[vectorTagPoints.size()-1].pos[0];
			lastPos[1] = vectorTagPoints[vectorTagPoints.size()-1].pos[1];
			lastPos[2] = vectorTagPoints[vectorTagPoints.size()-1].pos[2];
			//find two minmal distance;
			double minDis1,minDis2; minDis1 = minDis2 = DBL_MAX;
			double minP1[3], minP2[3];

			if(selectedTriangle == NULL){
				for(int i = 0; i < vectorTagPoints.size() - 1; i++){
					double dis = Distance(vectorTagPoints[i].pos, lastPos);
					if(dis < minDis1){
						minDis1 = dis;
						minP1[0] = vectorTagPoints[i].pos[0]; minP1[1] = vectorTagPoints[i].pos[1]; minP1[2] = vectorTagPoints[i].pos[2];
					}				
				}

				for(int i = 0; i < vectorTagPoints.size() - 1; i++){
					double dis = Distance(vectorTagPoints[i].pos, lastPos);
					if(dis < minDis2 && dis > minDis1){
						minDis2 = dis;
						minP2[0] = vectorTagPoints[i].pos[0]; minP2[1] = vectorTagPoints[i].pos[1]; minP2[2] = vectorTagPoints[i].pos[2];
					}
				}
			}
			else{
				for(int i = 0; i < vectorTagTriangles.size(); i++){
					if(vectorTagTriangles[i].triActor == selectedTriangle){
						double dis1 = Distance(vectorTagTriangles[i].p1, lastPos);
						double dis2 = Distance(vectorTagTriangles[i].p2, lastPos);
						double dis3 = Distance(vectorTagTriangles[i].p3, lastPos);
						minDis1 = std::min(dis1, std::min(dis2, dis3));
						//find minimum two points
						if(dis1 == minDis1){
							minDis2 = std::min(dis2,dis3);
							minP1[0] = vectorTagTriangles[i].p1[0];minP1[1] = vectorTagTriangles[i].p1[1];minP1[2] = vectorTagTriangles[i].p1[2];
							if(minDis2 == dis2){
								minP2[0] = vectorTagTriangles[i].p2[0];minP2[1] = vectorTagTriangles[i].p2[1];minP2[2] = vectorTagTriangles[i].p2[2];
							}
							else if(minDis2 == dis3){
								minP2[0] = vectorTagTriangles[i].p3[0];minP2[1] = vectorTagTriangles[i].p3[1];minP2[2] = vectorTagTriangles[i].p3[2];
							}
						}
						else if(dis2 == minDis1){
							minDis2 = std::min(dis1,dis3);
							minP1[0] = vectorTagTriangles[i].p2[0];minP1[1] = vectorTagTriangles[i].p2[1];minP1[2] = vectorTagTriangles[i].p2[2];
							if(minDis2 == dis1){
								minP2[0] = vectorTagTriangles[i].p1[0];minP2[1] = vectorTagTriangles[i].p1[1];minP2[2] = vectorTagTriangles[i].p1[2];
							}
							else{
								minP2[0] = vectorTagTriangles[i].p3[0];minP2[1] = vectorTagTriangles[i].p3[1];minP2[2] = vectorTagTriangles[i].p3[2];
							}
						}
						else if(dis3 == minDis1){
							minDis2 = std::min(dis1, dis2);
							minP1[0] = vectorTagTriangles[i].p3[0];minP1[1] = vectorTagTriangles[i].p3[1];minP1[2] = vectorTagTriangles[i].p3[2];
							if(minDis2 == dis1){
								minP2[0] = vectorTagTriangles[i].p1[0];minP2[1] = vectorTagTriangles[i].p1[1];minP2[2] = vectorTagTriangles[i].p1[2];
							}
							else{
								minP2[0] = vectorTagTriangles[i].p2[0];minP2[1] = vectorTagTriangles[i].p2[1];minP2[2] = vectorTagTriangles[i].p2[2];
							}
						}
					}
				}
			}

			vtkSmartPointer<vtkPoints> pts =
				vtkSmartPointer<vtkPoints>::New();
			pts->InsertNextPoint(lastPos[0], lastPos[1], lastPos[2]);
			pts->InsertNextPoint(minP1[0], minP1[1], minP1[2]);
			pts->InsertNextPoint(minP2[0], minP2[1], minP2[2]);			

			vtkSmartPointer<vtkTriangle> triangle =
				vtkSmartPointer<vtkTriangle>::New();
			triangle->GetPointIds()->SetId ( 0, 0 );
			triangle->GetPointIds()->SetId ( 1, 1 );
			triangle->GetPointIds()->SetId ( 2, 2 );

			vtkSmartPointer<vtkCellArray> triangles =
				vtkSmartPointer<vtkCellArray>::New();
			triangles->InsertNextCell ( triangle );

			// Create a polydata object
			vtkSmartPointer<vtkPolyData> trianglePolyData =
				vtkSmartPointer<vtkPolyData>::New();

			// Add the geometry and topology to the polydata
			trianglePolyData->SetPoints ( pts );
			trianglePolyData->SetPolys ( triangles );

			
			// Create mapper and actor
			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
			mapper->SetInput(trianglePolyData);
#else
			mapper->SetInputData(trianglePolyData);
#endif
			vtkSmartPointer<vtkActor> actor =
				vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			actor->GetProperty()->SetColor(triCol);

			//store triangle information in vector
			TagTriangle tri;
			tri.triActor = actor;
			tri.p1[0] = lastPos[0]; tri.p1[1] = lastPos[1]; tri.p1[2] = lastPos[2];
			tri.p2[0] = minP1[0]; tri.p2[1] = minP1[1]; tri.p2[2] = minP1[2];
			tri.p3[0] = minP2[0]; tri.p3[1] = minP2[1]; tri.p3[2] = minP2[2];
			tri.centerPos[0] = trianglePolyData->GetCenter()[0];
			tri.centerPos[1] = trianglePolyData->GetCenter()[1];
			tri.centerPos[2] = trianglePolyData->GetCenter()[2];
			vectorTagTriangles.push_back(tri);

			selectedTriangle = actor;
			SetSelectedTriColor();
			this->GetDefaultRenderer()->AddActor(actor);
		}
	}

	int ConstrainEdge(int type1, int type2)
	{
		// 1 = Branch point  2 = Free Edge point 3 = Interior point  4 = others
		//return 1;
		if(type1 == 1 && type1 == 1)
			return 3;
		else if(type1 == 2 && type2 == 2)
			return 1;
		else if(type1 == 3 && type2 == 3)
			return 2;
		else if((type1 == 1 && type2 == 2) || (type1 == 2 && type2 == 1))
			return 2;
		else if((type1 == 1 && type2 == 3) || (type1 == 1 && type2 == 3))
			return 2;
		else if((type1 == 2 && type2 == 3) || (type1 == 3 && type2 == 2))
			return 2;
	}

	int PairNumber(int a, int b){
		int a1 = std::min(a,b);
		int b1 = std::max(a,b);
		return (a1 + b1) * (a1 + b1 + 1) / 2.0 + b1;
	}

	void DrawTriangle()
	{
		int id1 = triPtIds[0], id2 = triPtIds[1], id3 = triPtIds[2];
		if(id1 == id2 || id1 == id3 || id2 == id3)
			return;
		int edgeid1 = PairNumber(triPtIds[0], triPtIds[1]);//id
		int edgeid2 = PairNumber(triPtIds[1], triPtIds[2]);
		int edgeid3 = PairNumber(triPtIds[2], triPtIds[0]);
		std::cout<<"ID "<<id1<<" "<<id2<<" "<<id3<<std::endl;
		//std::cout<<"Pair N "<<edgeid1<<" "<<edgeid2<<" "<<edgeid3<<std::endl;
		int cons1 = ConstrainEdge(vectorTagInfo[vectorTagPoints[id1].typeIndex].tagType, vectorTagInfo[vectorTagPoints[id2].typeIndex].tagType);
		int cons2 = ConstrainEdge(vectorTagInfo[vectorTagPoints[id2].typeIndex].tagType, vectorTagInfo[vectorTagPoints[id3].typeIndex].tagType);
		int cons3 = ConstrainEdge(vectorTagInfo[vectorTagPoints[id1].typeIndex].tagType, vectorTagInfo[vectorTagPoints[id3].typeIndex].tagType);

		if(vectorTagEdges[edgeid1].numEdge >= cons1)
			return;
		else if(vectorTagEdges[edgeid2].numEdge >= cons2)
			return;
		else if(vectorTagEdges[edgeid3].numEdge >= cons3)
			return;

		vectorTagEdges[edgeid1].numEdge++;
		vectorTagEdges[edgeid1].ptId1 = triPtIds[0]; vectorTagEdges[edgeid1].ptId2 = triPtIds[1];
		vectorTagEdges[edgeid2].numEdge++; 
		vectorTagEdges[edgeid2].ptId1 = triPtIds[1]; vectorTagEdges[edgeid2].ptId2 = triPtIds[2];
		vectorTagEdges[edgeid3].numEdge++; 
		vectorTagEdges[edgeid3].ptId1 = triPtIds[2]; vectorTagEdges[edgeid3].ptId2 = triPtIds[0];

		vtkSmartPointer<vtkPoints> pts =
			vtkSmartPointer<vtkPoints>::New();
		for(int i = 0; i < triPtIds.size(); i++){
			pts->InsertNextPoint(vectorTagPoints[triPtIds[i]].pos);
		}

		vtkSmartPointer<vtkTriangle> triangle =
			vtkSmartPointer<vtkTriangle>::New();
		triangle->GetPointIds()->SetId ( 0, 0 );
		triangle->GetPointIds()->SetId ( 1, 1 );
		triangle->GetPointIds()->SetId ( 2, 2 );

		vtkSmartPointer<vtkCellArray> triangles =
			vtkSmartPointer<vtkCellArray>::New();
		triangles->InsertNextCell ( triangle );

		// Create a polydata object
		vtkSmartPointer<vtkPolyData> trianglePolyData =
			vtkSmartPointer<vtkPolyData>::New();

		// Add the geometry and topology to the polydata
		trianglePolyData->SetPoints ( pts );
		trianglePolyData->SetPolys ( triangles );

		// Create mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
		mapper->SetInput(trianglePolyData);
#else
		mapper->SetInputData(trianglePolyData);
#endif
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(triCol);

		for(int i = 0; i < vectorTagTriangles.size(); i++)
		{
			if(vectorTagTriangles[i].centerPos[0] == actor->GetCenter()[0] &&
				vectorTagTriangles[i].centerPos[1] == actor->GetCenter()[1] &&
				vectorTagTriangles[i].centerPos[2] == actor->GetCenter()[2]){
				return;
				std::cout<<"same tri"<<std::endl;
			}
		}

		TagTriangle tri;
		tri.triActor = actor;
		tri.p1[0] = vectorTagPoints[triPtIds[0]].pos[0]; tri.p1[1] = vectorTagPoints[triPtIds[0]].pos[1]; tri.p1[2] = vectorTagPoints[triPtIds[0]].pos[2];
		tri.p2[0] = vectorTagPoints[triPtIds[1]].pos[0]; tri.p2[1] = vectorTagPoints[triPtIds[1]].pos[1]; tri.p2[2] = vectorTagPoints[triPtIds[1]].pos[2];
		tri.p3[0] = vectorTagPoints[triPtIds[2]].pos[0]; tri.p3[1] = vectorTagPoints[triPtIds[2]].pos[1]; tri.p3[2] = vectorTagPoints[triPtIds[2]].pos[2];
		tri.id1 = triPtIds[0]; tri.id2 = triPtIds[1]; tri.id3 = triPtIds[2];
		tri.centerPos[0] = trianglePolyData->GetCenter()[0];
		tri.centerPos[1] = trianglePolyData->GetCenter()[1];
		tri.centerPos[2] = trianglePolyData->GetCenter()[2];
		vectorTagTriangles.push_back(tri);

		this->GetDefaultRenderer()->AddActor(actor);
	}

	void DeleteTriangle(vtkActor* pickedActor){
		if(pickedActor != NULL)
		{
			pickedActor->GetProperty()->SetColor(0,0,0);
			for(int i = 0; i < vectorTagTriangles.size(); i++)
			{
				TagTriangle tri = vectorTagTriangles[i];
				if(pickedActor == tri.triActor)
				{
					vectorTagEdges[PairNumber(vectorTagTriangles[i].id1, vectorTagTriangles[i].id2)].numEdge--;
					vectorTagEdges[PairNumber(vectorTagTriangles[i].id3, vectorTagTriangles[i].id2)].numEdge--;
					vectorTagEdges[PairNumber(vectorTagTriangles[i].id1, vectorTagTriangles[i].id3)].numEdge--;
					vectorTagTriangles.erase(vectorTagTriangles.begin() + i);
					this->GetDefaultRenderer()->RemoveActor(pickedActor);
					i--;
				}
			}
		}
	}
	
	vtkActor* PickAcotrFromMesh(double pos[3])
	{
		vtkSmartPointer<vtkActor> pickedActor
			= vtkSmartPointer<vtkActor>::New();

		vtkActorCollection* actorsCollection = this->GetDefaultRenderer()->GetActors();
		actorsCollection->InitTraversal();

		std::cout<<"number of actors: "<<actorsCollection->GetNumberOfItems()<<std::endl;
		//find the point to delete
		for(vtkIdType i = 0; i < actorsCollection->GetNumberOfItems(); i++)
		{
			vtkActor* nextActor = actorsCollection->GetNextActor();
			double* actorPos = nextActor->GetCenter();
			double dis = Distance(pos,actorPos);

			if(dis < 1.0 && nextActor){
				pickedActor = nextActor;
				return pickedActor;
			}
		}
		return NULL;
	}

	void copyEdgeBtoA(int a, int b){
		vectorTagEdges[a].constrain = vectorTagEdges[b].constrain;
		vectorTagEdges[a].numEdge = vectorTagEdges[b].numEdge;
	}

	int deleteEdgeHelper(int id1, int id2, int seq)
	{
		if(id1 < seq && id2 < seq)
			return PairNumber(id1, id2);
		else if(id1 < seq && id2 > seq)
			return PairNumber(id1, id2-1);
		else if(id1 > seq && id2 < seq)
			return PairNumber(id1-1, id2);
		else if(id1 > seq && id2 > seq)
			return PairNumber(id1 - 1 , id2 - 1);
	}

	int deleteEdgeHelper2(int id, int seq)
	{
		if(id > seq)
			return id - 1;
		else
			return id;
	}

	void deleteEdge(int seq)
	{
		std::vector<TagEdge> temp;
		std::vector<int> zeroEdgeId;
		for(int i = 0; i < vectorTagTriangles.size(); i++)
		{			
			int id1 = vectorTagTriangles[i].id1, id2 = vectorTagTriangles[i].id2, id3 = vectorTagTriangles[i].id3;
			int nori;			
			int n;
			TagEdge e1;
			if(id1 == seq || id2 == seq || id3 == seq)//if the point is belongs to triangle
			{
				int d1, d2;
				if(id1 == seq)
				{
					d1 = PairNumber(id1, id2);
					d2 = PairNumber(id1, id3);
					nori = PairNumber(id2,id3);
					n = deleteEdgeHelper(id2,id3,seq);
				}
				else if(id2 == seq)
				{
					d1 = PairNumber(id2, id3);
					d2 = PairNumber(id2, id1);
					nori = PairNumber(id1,id3);
					n = deleteEdgeHelper(id1,id3,seq);
				}
				else if(id3 == seq)
				{
					d1 = PairNumber(id3, id1);
					d2 = PairNumber(id3, id2);
					nori = PairNumber(id1,id2);
					n = deleteEdgeHelper(id1,id2,seq);
				}

				e1.seq = n;
				e1.constrain = vectorTagEdges[nori].constrain;
				e1.numEdge = --vectorTagEdges[nori].numEdge;
				e1.ptId1 = deleteEdgeHelper2(vectorTagEdges[nori].ptId1, seq);
				e1.ptId2 = deleteEdgeHelper2(vectorTagEdges[nori].ptId2, seq);
				temp.push_back(e1);
				zeroEdgeId.push_back(nori);
				zeroEdgeId.push_back(d1);
				zeroEdgeId.push_back(d2);
				//std::cout<<"0 id"<<nori<<" "<<d1<<" "<<d2<<std::endl;
				this->GetDefaultRenderer()->RemoveActor(vectorTagTriangles[i].triActor);
				vectorTagTriangles.erase(vectorTagTriangles.begin() + i);
				i--;
			}
			else
			{
				int minId = std::min(id1, std::min(id2, id3));
				int maxId = std::max(id1, std::max(id2, id3));
				if(maxId > seq)
				{

					for(int j = 0; j < 3; j++)
					{
						if(j == 0){
							nori = PairNumber(id1,id2);
							n = deleteEdgeHelper(id1,id2,seq);
						}
						else if(j == 1){
							nori = PairNumber(id3,id2);
							n = deleteEdgeHelper(id3,id2,seq);
						}
						else if(j == 2){
							nori = PairNumber(id3,id1);
							n = deleteEdgeHelper(id3,id1,seq);
						}
						e1.seq = n;
						e1.constrain = vectorTagEdges[nori].constrain;
						e1.numEdge = vectorTagEdges[nori].numEdge;
						e1.ptId1 = deleteEdgeHelper2(vectorTagEdges[nori].ptId1, seq);
						e1.ptId2 = deleteEdgeHelper2(vectorTagEdges[nori].ptId2, seq);
						temp.push_back(e1);
						zeroEdgeId.push_back(nori);
					}
				}
				vectorTagTriangles[i].id1 = deleteEdgeHelper2(id1,seq);
				vectorTagTriangles[i].id2 = deleteEdgeHelper2(id2,seq);
				vectorTagTriangles[i].id3 = deleteEdgeHelper2(id3,seq);
			}			
		}

		for (int i = 0; i < zeroEdgeId.size(); i++)
		{
			int id = zeroEdgeId[i];
			vectorTagEdges[id].constrain = vectorTagEdges[id].numEdge = vectorTagEdges[id].ptId1 = vectorTagEdges[id].ptId2 = 0;
		}

		for(int j = 0; j < temp.size(); j++)
		{
			int nu = temp[j].seq;
			vectorTagEdges[nu].constrain = temp[j].constrain;
			vectorTagEdges[nu].numEdge = temp[j].numEdge;
			vectorTagEdges[nu].ptId1 = temp[j].ptId1;
			vectorTagEdges[nu].ptId2 = temp[j].ptId2;
		}
		
	}

	virtual void OnLeftButtonDown()
	{		
		int* clickPos = this->GetInteractor()->GetEventPosition();
		// Pick from this location.
		vtkSmartPointer<vtkPropPicker>  picker =
			vtkSmartPointer<vtkPropPicker>::New();

		int sucessPick = picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());
		if(sucessPick != 0){//pick successful
			double* pos = picker->GetPickPosition();
						 
			//find the first actor
			vtkSmartPointer<vtkActorCollection> actors = this->GetDefaultRenderer()->GetActors();
			vtkSmartPointer<vtkActor> actor0 =  static_cast<vtkActor *>(actors->GetItemAsObject(0));	
			vtkRenderWindowInteractor *rwi = this->Interactor;
			if(rwi->GetKeySym() != NULL && isSkeleton)
			{
				std::string key = rwi->GetKeySym();
				//for the adding point event
				if( rwi->GetControlKey() && pos[0] != 0 && pos[1] != 0 && pos[2] != 0)
				{
					//see if there is any tag have been added
					if(vectorTagInfo.size() != 0)
					{
						vtkSmartPointer<vtkDataSet> vtkdata = actor0->GetMapper()->GetInputAsDataSet();
						//vtkdata->GetPointData()->GetAbstractArray("Radius");
						vtkDoubleArray* radiusArray = (vtkDoubleArray*)vtkdata->GetPointData()->GetArray("Radius");
						double minDistance = DBL_MAX;
						double finalPos[3];
						double pointRadius;
						int pointSeq;
						//finalPos[0] = pos[0]; finalPos[1] = pos[1]; finalPos[2] = pos[2];
						for(vtkIdType i = 0; i < vtkdata->GetNumberOfPoints(); i++){
							double p[3];
							vtkdata->GetPoint(i,p);
							double dist = std::sqrt(std::pow(pos[0] - p[0],2) + std::pow(pos[1] - p[1], 2) + std::pow(pos[2] - p[2], 2));
							if(dist < minDistance){
								minDistance = dist;
								pointSeq = i;
								finalPos[0] = p[0]; finalPos[1] = p[1]; finalPos[2] = p[2];
								pointRadius = radiusArray->GetValue(i);
							}
						}
						if(labelData[pointSeq] == 0.0){
							std::cout << "Pick position (final position) is: "
								<< finalPos[0] << " " << finalPos[1]
							<< " " << finalPos[2] << std::endl;

							//draw line if the number of selected points is more than 2
							//DrawDelaunayTriangle();
							//DrawRegularTriangle();

							//Create a sphere
							vtkSmartPointer<vtkSphereSource> sphereSource =
								vtkSmartPointer<vtkSphereSource>::New();
							sphereSource->SetCenter(finalPos[0], finalPos[1], finalPos[2]);
							sphereSource->SetRadius(1.0);

							//Create a mapper and actor
							vtkSmartPointer<vtkPolyDataMapper> mapper =
								vtkSmartPointer<vtkPolyDataMapper>::New();
							mapper->SetInputConnection(sphereSource->GetOutputPort());

							QComboBox* cbTagPoint =  qtObject->getTagComboBox();
							TagInfo ti = vectorTagInfo[cbTagPoint->currentIndex()];
							vtkSmartPointer<vtkActor> actor =
								vtkSmartPointer<vtkActor>::New();
							actor->SetMapper(mapper);
							actor->GetProperty()->SetColor(ti.tagColor[0] / 255.0, ti.tagColor[1] / 255.0, ti.tagColor[2] / 255.0);
						
							//store actor in vectorTagPoints
							TagPoint actorT;
							actorT.actor = actor;
							actorT.typeIndex = cbTagPoint->currentIndex();
							actorT.typeName = cbTagPoint->currentText().toStdString(); 
							actorT.radius = pointRadius;
							actorT.seq = pointSeq;
							actorT.pos[0] = finalPos[0]; actorT.pos[1] = finalPos[1]; actorT.pos[2] = finalPos[2];
							vectorTagPoints.push_back(actorT);

							//calculate the biggest number of edges possibility
							vectorTagEdges.resize(PairNumber(vectorTagPoints.size(), vectorTagPoints.size()));
							
							labelData[pointSeq] = cbTagPoint->currentIndex() + 1;
						
							if(vectorClassifyPoints.size() <= cbTagPoint->currentIndex()){
								vectorClassifyPoints.resize(cbTagPoint->currentIndex() + 1);
							}
							vectorClassifyPoints[cbTagPoint->currentIndex()].push_back(actorT);

							this->GetDefaultRenderer()->AddActor(actor);
						}
					}
				}//end of control

				//for delete point event
				else if(key.compare("s") == 0)
				{
					rwi->SetKeySym("");

					vtkSmartPointer<vtkActor> pickedActor
						= vtkSmartPointer<vtkActor>::New();

					pickedActor = PickAcotrFromMesh(pos);

					//erase from vectorTagPoints and color other points
					for(int i = 0; i < vectorTagPoints.size(); i++){
						TagPoint at = vectorTagPoints[i];
						double* acotrPos = at.actor->GetPosition();
						if(pickedActor != at.actor){							
							at.actor->GetProperty()->SetColor(vectorTagInfo[at.typeIndex].tagColor[0] / 255.0,
								vectorTagInfo[at.typeIndex].tagColor[1] / 255.0, 
								vectorTagInfo[at.typeIndex].tagColor[2] / 255.0);
						}					
						else{
							/*for(int j = 0; j < vectorClassifyPoints[vectorTagPoints[i].typeIndex].size(); j++){
								if(vectorClassifyPoints[vectorTagPoints[i].typeIndex][j].actor == at.actor){
									vectorClassifyPoints[vectorTagPoints[i].typeIndex].erase(vectorClassifyPoints[vectorTagPoints[i].typeIndex].begin()
										+ j);
								}
							}*/
							labelData[vectorTagPoints[i].seq] = 0.0;							
							deleteEdge(i);
							
							vectorTagPoints.erase(vectorTagPoints.begin() + i);

							////find triangle and delete triangle
							/*for(int j = 0; j < vectorTagTriangles.size(); j++)
							{
								if(vectorTagTriangles[j].id1 == i || vectorTagTriangles[j].id2 == i || vectorTagTriangles[j].id3 == i){
									//DeleteTriangle(vectorTagTriangles[j].triActor);
									this->GetDefaultRenderer()->RemoveActor(vectorTagTriangles[j].triActor);
									vectorTagTriangles.erase(vectorTagTriangles.begin() + j);
									j--;
								}
							}*/

							vectorTagEdges.resize(PairNumber(vectorTagPoints.size(), vectorTagPoints.size()));
							this->GetDefaultRenderer()->RemoveActor(pickedActor);
							//DrawDelaunayTriangle();
							//DrawRegularTriangle();
							i--;
						}
					}
					std::cout<<"you are out"<<std::endl;
				}//end of s

				
				else if(key.compare("t") == 0)
				{
					rwi->SetKeySym("");
					std::cout<<"Tri in"<<std::endl;
					vtkActor *actor = picker->GetActor();
					vtkActorCollection* acotrCollection = this->GetDefaultRenderer()->GetActors();
					acotrCollection->InitTraversal();
					
					if(actor != acotrCollection->GetNextActor()){//except for the skeleton
						for(int i = 0; i < vectorTagTriangles.size(); i++){
							if(actor == vectorTagTriangles[i].triActor){
								selectedTriangle = actor;
								selectedTriangle->GetProperty()->SetColor(1,1,0);
							}
							else{
								vectorTagTriangles[i].triActor->GetProperty()->SetColor(triCol);
							}
						}												
					}					
				}//end of t

				else if(key.compare("q") == 0)
				{
					rwi->SetKeySym("");

					vtkSmartPointer<vtkActor> pickedActor
						= vtkSmartPointer<vtkActor>::New();

					pickedActor = PickAcotrFromMesh(pos);

					if(pickedActor != NULL){
						pickedActor->GetProperty()->SetColor(1,1,1);

						for(int i = 0; i < vectorTagPoints.size(); i++){
							TagPoint at = vectorTagPoints[i];
							if(pickedActor == at.actor){	
								triPtIds.push_back(i);
							}
						}

						if(triPtIds.size() == 3){
							DrawTriangle();
							for(int i = 0; i < triPtIds.size(); i++){
								TagPoint at = vectorTagPoints[triPtIds[i]];
								vectorTagPoints[triPtIds[i]].actor->GetProperty()->SetColor(vectorTagInfo[at.typeIndex].tagColor[0] / 255.0,
									vectorTagInfo[at.typeIndex].tagColor[1] / 255.0, 
									vectorTagInfo[at.typeIndex].tagColor[2] / 255.0);
							}

							triPtIds.resize(0);
						}
					}
				}// end of q


				else if(key.compare("d") == 0)
				{
					rwi->SetKeySym("");

					vtkSmartPointer<vtkActor> pickedActor
						= vtkSmartPointer<vtkActor>::New();

					pickedActor = PickAcotrFromMesh(pos);
					DeleteTriangle(pickedActor);
				}
				/*else if(key.compare("p") == 0){
					vtkSmartPointer<vtkActor> pickedActor
						= vtkSmartPointer<vtkActor>::New();

					pickedActor = PickAcotrFromMesh(pos);

					for(int i = 0; i < vectorTagPoints.size(); i++){
						TagPoint at = vectorTagPoints[i];
						double* acotrPos = at.actor->GetPosition();
						if(pickedActor == at.actor){	
							at.actor->GetProperty()->SetColor(0,0,0);
							deletePointIds.push_back(i);
						}
					}
				}


				else if(key.compare("i") == 0){
					DrawDelaunayTriangle();
				}*/
			}//end of key press
		}
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();				
	}
private:
	double triCol[3];
};
vtkStandardNewMacro(MouseInteractorAdd);

// Constructor
EventQtSlotConnect::EventQtSlotConnect()
{
  this->setupUi(this);

  this->Connections = vtkSmartPointer<vtkEventQtSlotConnect>::New();

  this->cmrep_progressBar->setMinimum(0);
  this->cmrep_progressBar->setMaximum(0);
  this->cmrep_progressBar->hide();

  createActions();
  createMenus();

  pointColor[0] = 1; pointColor[1] = 0; pointColor[2] = 0;

  this->connect(&this->FutureWatcher, SIGNAL(finished()), this, SLOT(slot_finished()));
  this->connect(this->cmrepVskel, SIGNAL(clicked()), this, SLOT(executeCmrepVskel()));
  this->connect(this->checkBoxHideSkel, SIGNAL(stateChanged(int)), this, SLOT(slot_skelStateChange(int)));
  this->connect(this->pushButtonAddTag, SIGNAL(clicked()), this, SLOT(slot_addTag()));
};

void EventQtSlotConnect::slot_addTag(){

	AddTagDialog addDialog;
	addDialog.show();

	for(int i = 0; i < vectorTagInfo.size(); i++){
		addDialog.setTagName(vectorTagInfo[i].tagName);
		addDialog.setTagColor(vectorTagInfo[i].qc);
	}

	if(addDialog.exec()){
		QString tagText = addDialog.lineEdit->text();
		
		TagInfo ti;
		ti.tagName = tagText.toStdString();
		ti.qc = addDialog.color;
		int r, g, b;
		r = addDialog.color.red();
		g = addDialog.color.green();
		b = addDialog.color.blue();
		//ti.tagColor[0] = ((double) std::rand() / (RAND_MAX)); ti.tagColor[1] = ((double) std::rand() / (RAND_MAX)); ti.tagColor[2] = ((double) std::rand() / (RAND_MAX));
		ti.tagColor[0] = r; ti.tagColor[1] = g; ti.tagColor[2] = b;
		if(addDialog.branchButton->isChecked())
			ti.tagType = 1;
		else if(addDialog.freeEdgeButton->isChecked())
			ti.tagType = 2;
		else if(addDialog.interiorButton->isChecked())
			ti.tagType = 3;
		else
			ti.tagType = 4;

		vectorTagInfo.push_back(ti);

		QPixmap pix(22,22);
		int iSize = vectorTagInfo.size();
		std::stringstream ss;
		ss << iSize;
		std::string num = ss.str() + " ";
		QString displayText = QString(num.c_str()) + " " + tagText;
		pix.fill(addDialog.color);
		this->comboBoxTagPoint->addItem(pix, displayText);
	}	
}


void EventQtSlotConnect::slot_finished()
{
	readVTK(VTKfilename);
	this->cmrep_progressBar->hide();
}

void EventQtSlotConnect::slot_position(double x, double y, double z)
{
	textEdit->append("Position");
}

void EventQtSlotConnect::slot_clicked(vtkObject*, unsigned long, void*, void*)
{ 
 // textEdit->append("Clicked");
}


void EventQtSlotConnect::slot_open(){
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
	if (!fileName.isEmpty()) {
		VTKfilename = fileName.toStdString();
		readVTK(VTKfilename);
	}
}

void EventQtSlotConnect::slot_save(){

	QFileDialog::Options options;
	//if (!native->isChecked())
	//	options |= QFileDialog::DontUseNativeDialog;
	QString selectedFilter;
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("QFileDialog::getSaveFileName()"),
		tr(""),
		tr("VTK Files (*.vtk)"),
		&selectedFilter,
		options);
	

	vtkSmartPointer<vtkGenericDataObjectWriter> writer = 
		vtkSmartPointer<vtkGenericDataObjectWriter>::New();
	if (!fileName.isEmpty())
		writer->SetFileName(fileName.toStdString().c_str());

		
	/*vtkSmartPointer<vtkAppendPolyData> appPolyData = 
		vtkSmartPointer<vtkAppendPolyData>::New();

	appPolyData->AddInput(polyObject);
	for(int i = 0; i < vectorTagTriangles.size(); i++)
	{
		appPolyData->AddInput((vtkPolyData*)vectorTagTriangles[i].triActor->GetMapper()->GetInput());
	}*/

	vtkSmartPointer<vtkPolyData> finalPolyData =
		vtkSmartPointer<vtkPolyData>::New();

	finalPolyData = polyObject;

	vtkSmartPointer<vtkDoubleArray> dblArray1 = 
		vtkSmartPointer<vtkDoubleArray>::New();
	dblArray1->SetName("Label");
	for(int i = 0; i < labelData.size(); i++)
		dblArray1->InsertNextValue(labelData[i]);
	finalPolyData->GetPointData()->AddArray(dblArray1);

	vtkSmartPointer<vtkDoubleArray> dblArray2 = 
		vtkSmartPointer<vtkDoubleArray>::New();
	dblArray2->SetName("TagTriangles");
	for(int i = 0; i < vectorTagTriangles.size(); i++)
	{
		dblArray2->InsertNextValue(vectorTagTriangles[i].p1[0]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p1[1]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p1[2]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].id1);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p2[0]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p2[1]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p2[2]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].id2);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p3[0]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p3[1]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].p3[2]);
		dblArray2->InsertNextValue(vectorTagTriangles[i].id3);
	}
	finalPolyData->GetPointData()->AddArray(dblArray2);

	vtkSmartPointer<vtkDoubleArray> dblArray3 = 
		vtkSmartPointer<vtkDoubleArray>::New();
	dblArray3->SetName("TagEdges");
	for(int i = 0; i < vectorTagEdges.size(); i++)
	{
		dblArray3->InsertNextValue(vectorTagEdges[i].ptId1);
		dblArray3->InsertNextValue(vectorTagEdges[i].ptId2);
		dblArray3->InsertNextValue(vectorTagEdges[i].seq);
		dblArray3->InsertNextValue(vectorTagEdges[i].numEdge);
		dblArray3->InsertNextValue(vectorTagEdges[i].constrain);
	}
	finalPolyData->GetPointData()->AddArray(dblArray3);

	vtkSmartPointer<vtkDoubleArray> dblArray4 = 
		vtkSmartPointer<vtkDoubleArray>::New();
	dblArray4->SetName("TagPoints");
	for(int i = 0; i < vectorTagPoints.size(); i++)
	{
		dblArray4->InsertNextValue(vectorTagPoints[i].pos[0]);
		dblArray4->InsertNextValue(vectorTagPoints[i].pos[1]);
		dblArray4->InsertNextValue(vectorTagPoints[i].pos[2]);
		dblArray4->InsertNextValue(vectorTagPoints[i].radius);
		dblArray4->InsertNextValue(vectorTagPoints[i].seq);
		dblArray4->InsertNextValue(vectorTagPoints[i].typeIndex);
	}
	finalPolyData->GetPointData()->AddArray(dblArray4);

	vtkSmartPointer<vtkDoubleArray> dblArray5 = 
		vtkSmartPointer<vtkDoubleArray>::New();
	dblArray5->SetName("TagInfo");
	for(int i = 0; i < vectorTagInfo.size(); i++)
	{
		dblArray5->InsertNextValue(vectorTagInfo[i].tagType);
		dblArray5->InsertNextValue(vectorTagInfo[i].tagColor[0]);
		dblArray5->InsertNextValue(vectorTagInfo[i].tagColor[1]);
		dblArray5->InsertNextValue(vectorTagInfo[i].tagColor[2]);
	}
	finalPolyData->GetPointData()->AddArray(dblArray5);
	/*for(int i = 0; i < vectorTagInfo.size(); i++)
	{
	vtkSmartPointer<vtkDoubleArray> dblArray1 = 
	vtkSmartPointer<vtkDoubleArray>::New();
	vtkSmartPointer<vtkDoubleArray> dblArray2 = 
	vtkSmartPointer<vtkDoubleArray>::New();

	dblArray1->SetName(vectorTagInfo[i].tagName.c_str());
	dblArray2->SetName((vectorTagInfo[i].tagName + "_Radius").c_str());

	vtkSmartPointer<vtkPoints> pts =
	vtkSmartPointer<vtkPoints>::New();
	for(int j = 0; j < vectorClassifyPoints[i].size(); j++)
	{			
	dblArray1->InsertNextValue(vectorClassifyPoints[i][j].actor->GetCenter()[0]);
	dblArray1->InsertNextValue(vectorClassifyPoints[i][j].actor->GetCenter()[1]);
	dblArray1->InsertNextValue(vectorClassifyPoints[i][j].actor->GetCenter()[2]);	
	dblArray2->InsertNextValue(vectorClassifyPoints[i][j].radius);
	}
	polyObject->GetPointData()->AddArray(dblArray1);
	polyObject->GetPointData()->AddArray(dblArray2);
	}*/

	writer->SetInput(finalPolyData);
	//writer->SetInput(polyObject);
	writer->Update();
	writer->Write();
}

void EventQtSlotConnect::slot_skelStateChange(int state){
	std::cout<<"state changed"<<std::endl;
	vtkRendererCollection* rendercollection = this->qvtkWidget->GetRenderWindow()->GetRenderers();
	//rendercollection->InitTraversal();
	vtkRenderer* render = rendercollection->GetFirstRenderer();
	vtkActorCollection* actorcollection = render->GetActors();
	actorcollection->InitTraversal();
	vtkActor* actor = actorcollection->GetNextActor();
	if(state == Qt::Unchecked)
		actor->VisibilityOn();
	else
		actor->VisibilityOff();
	//render->ResetCamera();
	this->qvtkWidget->GetRenderWindow()->Render();
}

void EventQtSlotConnect::executeCmrepVskel()
{

	std::vector <char *> parameters;
	parameters.push_back("cmrep_vskel");
	
	QString pathTextQ = this->pathParameter->text();
	std::string pathText = pathTextQ.toStdString();
	if(!pathText.empty()){
		parameters.push_back("-Q");
		char *temp = new char;
		strcpy(temp, pathText.c_str());
		parameters.push_back(temp);
	}
	
	int evalue = this->eParameter->value();
	if(evalue != 0){
		parameters.push_back("-e");
		char *temp = new char;
		itoa(evalue, temp, 10);
		parameters.push_back(temp);
		delete temp;
	}

	double pvalue = this->pParameter->value();
	if(pvalue != 0.0){
		parameters.push_back("-p");
		char *temp = new char;
		std::stringstream ss;
		ss << pvalue;
		std::string tempS = ss.str();
		strcpy(temp, tempS.c_str());
		parameters.push_back(temp);
		delete temp;
	}

	int cvalue = this->cParameter->value();
	if(cvalue != 0){
		parameters.push_back("-c");
		char *temp = new char;
		itoa(cvalue, temp, 10);
		parameters.push_back(temp);
		delete temp;
	}

	int tvalue = this->tParameter->value();
	if(tvalue != 0){
		parameters.push_back("-t");
		char *temp = new char;
		itoa(tvalue, temp, 10);
		parameters.push_back(temp);
		delete temp;
	}

	QString stextQ = this->sParameter->text();
	std::string stext = stextQ.toStdString();
	if(!stext.empty()){
		parameters.push_back("-s");
		char *temp = new char;
		strcpy(temp, stext.c_str());
		parameters.push_back(temp);
		delete temp;
	}

	QString RtextQ = this->RParameter->text();
	std::string Rtext = RtextQ.toStdString();
	if(!Rtext.empty()){
		parameters.push_back("-R");
		char *temp = new char;
		strcpy(temp, Rtext.c_str());
		parameters.push_back(temp);
		delete temp;
	}

	QString TtextQ = this->TParameter->text();
	std::string Ttext = TtextQ.toStdString();
	if(!Ttext.empty()){
		parameters.push_back("-T");
		char *temp = new char;
		strcpy(temp, Ttext.c_str());
		parameters.push_back(temp);
		delete temp;
	}

	QString ItextQ = this->IParameter->text();
	std::string Itext = ItextQ.toStdString();
	if(!Itext.empty()){
		parameters.push_back("-I");
		char *temp = new char;
		strcpy(temp, Itext.c_str());
		parameters.push_back(temp);
		delete temp;
	}

	QString qtextQ = this->qParameter->text();
	std::string qtext = qtextQ.toStdString();
	if(!qtext.empty()){
		parameters.push_back("-q");
		char *temp = new char;
		strcpy(temp, qtext.c_str());
		parameters.push_back(temp);
		delete temp;
	}
	
	char *command[3];
	command[1] = new char[VTKfilename.length() + 1];
	strcpy(command[1], VTKfilename.c_str());
	parameters.push_back(command[1]);

	std::string outputNameSkel =  VTKfilename;
	outputNameSkel = outputNameSkel.substr(0, outputNameSkel.length() - 4) + "_Skel.vtk";
	command[2] = new char [outputNameSkel.length() + 1];
	strcpy(command[2], outputNameSkel.c_str());
	parameters.push_back(command[2]);


	/*std::cout<<"parameter"<<std::endl;
	for(int i = 0; i < parameters.size(); i++)
		std::cout<<parameters[i]<<std::endl;*/
	this->cmrep_progressBar->show();
	QFuture<void> future = QtConcurrent::run(&this->v, &VoronoiSkeletonTool::execute, parameters.size(), parameters);
	this->FutureWatcher.setFuture(future);
	//v.execute(parameters.size(), parameters);

	VTKfilename = outputNameSkel;
}

void EventQtSlotConnect::createActions(){
	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(slot_open()));

	saveAct = new QAction(tr("&Save"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(slot_save()));
}

void EventQtSlotConnect::createMenus(){
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	menuBar()->addMenu(fileMenu);
}

QComboBox* EventQtSlotConnect::getTagComboBox(){
	return this->comboBoxTagPoint;
}


void EventQtSlotConnect::readCustomDataLabel(vtkDoubleArray* labelDBL)
{
	labelData.clear();
	for(int i = 0; i < labelDBL->GetSize(); i++)
	{
		labelData.push_back(labelDBL->GetValue(i));
	}
}

void EventQtSlotConnect::readCustomDataTri(vtkDoubleArray* triDBL)
{
	for(vtkIdType i = 0; i < triDBL->GetSize();)
	{
		//triDBL->GetValue(i);
		vtkSmartPointer<vtkPoints> pts =
			vtkSmartPointer<vtkPoints>::New();

		TagTriangle tri;
		tri.p1[0] = triDBL->GetValue(i);tri.p1[1] = triDBL->GetValue(i+1);tri.p1[2] = triDBL->GetValue(i+2);
		tri.id1 = triDBL->GetValue(i+3);
		tri.p2[0] = triDBL->GetValue(i+4);tri.p1[1] = triDBL->GetValue(i+5);tri.p1[2] = triDBL->GetValue(i+6);
		tri.id2 = triDBL->GetValue(i+7);
		tri.p3[0] = triDBL->GetValue(i+8);tri.p1[1] = triDBL->GetValue(i+9);tri.p1[2] = triDBL->GetValue(i+10);
		tri.id3 = triDBL->GetValue(i+11);


		for(int j = 0; j < 3; j++){
			int t1,t2,t3;
			t1 = triDBL->GetValue(i); t2 = triDBL->GetValue(i+1); t3 = triDBL->GetValue(i+2);
			pts->InsertNextPoint(t1,t2,t3);
			i += 4;
		}

		vtkSmartPointer<vtkTriangle> triangle =
			vtkSmartPointer<vtkTriangle>::New();
		triangle->GetPointIds()->SetId ( 0, 0 );
		triangle->GetPointIds()->SetId ( 1, 1 );
		triangle->GetPointIds()->SetId ( 2, 2 );

		vtkSmartPointer<vtkCellArray> triangles =
			vtkSmartPointer<vtkCellArray>::New();
		triangles->InsertNextCell ( triangle );

		// Create a polydata object
		vtkSmartPointer<vtkPolyData> trianglePolyData =
			vtkSmartPointer<vtkPolyData>::New();

		// Add the geometry and topology to the polydata
		trianglePolyData->SetPoints ( pts );
		trianglePolyData->SetPolys ( triangles );

		// Create mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
		mapper->SetInput(trianglePolyData);
#else
		mapper->SetInputData(trianglePolyData);
#endif
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		double triCol[3];
		triCol[0] = 0.2; triCol[1] = 0.2; triCol[2] = 0.7;
		actor->GetProperty()->SetColor(triCol);

		tri.centerPos[0] = actor->GetCenter()[0];
		tri.centerPos[1] = actor->GetCenter()[1];
		tri.centerPos[2] = actor->GetCenter()[2];
		tri.triActor = actor;
		vectorTagTriangles.push_back(tri);
		this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	}
}

void EventQtSlotConnect::readCustomDataEdge(vtkDoubleArray* edgeDBL)
{
	for(int i = 0; i < edgeDBL->GetSize(); i += 5)
	{
		TagEdge edge;
		edge.ptId1 = edgeDBL->GetValue(i);
		edge.ptId2 = edgeDBL->GetValue(i+1);
		edge.seq = edgeDBL->GetValue(i+2);
		edge.numEdge = edgeDBL->GetValue(i+3);
		edge.constrain = edgeDBL->GetValue(i+4);

		vectorTagEdges.push_back(edge);
	}
}

void EventQtSlotConnect::readCustomDataTag(vtkDoubleArray* tagDBL)
{
	for(int i = 0; i < tagDBL->GetSize(); i += 4)
	{
		TagInfo info;
		info.tagType = tagDBL->GetValue(i);
		info.tagColor[0] = tagDBL->GetValue(i+1);
		info.tagColor[1] = tagDBL->GetValue(i+2);
		info.tagColor[2] = tagDBL->GetValue(i+3);
		info.qc = QColor(info.tagColor[0], info.tagColor[1], info.tagColor[2]);
		vectorTagInfo.push_back(info);

		QPixmap pix(22,22);
		int iSize = vectorTagInfo.size();
		std::stringstream ss;
		ss << iSize;
		std::string num = ss.str() + " ";
		QString displayText = QString(num.c_str()) + " " + "";
		pix.fill(info.qc);
		this->comboBoxTagPoint->addItem(pix, displayText);
	}
}

void EventQtSlotConnect::readCustomDataPoints(vtkDoubleArray* ptsDBL)
{
	for(int i = 0; i < ptsDBL->GetSize(); i += 6)
	{
		TagPoint tagPt;
		tagPt.pos[0] = ptsDBL->GetValue(i);
		tagPt.pos[1] = ptsDBL->GetValue(i+1);
		tagPt.pos[2] = ptsDBL->GetValue(i+2);
		tagPt.radius = ptsDBL->GetValue(i+3);
		tagPt.seq = ptsDBL->GetValue(i+4);
		tagPt.typeIndex = ptsDBL->GetValue(i+5);

		//Create a sphere
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->SetCenter(tagPt.pos[0], tagPt.pos[1], tagPt.pos[2]);
		sphereSource->SetRadius(1.0);

		//Create a mapper and actor
		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(sphereSource->GetOutputPort());

		//QComboBox* cbTagPoint =  qtObject->getTagComboBox();
		TagInfo ti = vectorTagInfo[tagPt.typeIndex];
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(ti.tagColor[0] / 255.0, ti.tagColor[1] / 255.0, ti.tagColor[2] / 255.0);

		tagPt.actor = actor;
		vectorTagPoints.push_back(tagPt);
		this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	}
}

void EventQtSlotConnect::readCustomData(vtkPolyData *polydata)
{
	vtkDoubleArray* labelDBL = (vtkDoubleArray*)polydata->GetPointData()->GetArray("Label");
	readCustomDataLabel(labelDBL);

	vtkDoubleArray* triDBL = (vtkDoubleArray*)polydata->GetPointData()->GetArray("TagTriangles");
	readCustomDataTri(triDBL);

	vtkDoubleArray* edgeDBL = (vtkDoubleArray*)polydata->GetPointData()->GetArray("TagEdges");
	readCustomDataEdge(edgeDBL);

	vtkDoubleArray* tagDBL = (vtkDoubleArray*)polydata->GetPointData()->GetArray("TagInfo");
	readCustomDataTag(tagDBL);

	vtkDoubleArray* ptsDBL = (vtkDoubleArray*)polydata->GetPointData()->GetArray("TagPoints");
	readCustomDataPoints(ptsDBL);
}

void EventQtSlotConnect::readVTK(std::string filename){
	std::string inputFilename = filename;

	vtkSmartPointer<vtkGenericDataObjectReader> reader = 
	  vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

	// Create a polydata object
	//vtkPolyData* polydata = planeSource->GetOutput();
    vtkPolyData* polydata =  reader->GetPolyDataOutput();
	/*vtkSmartPointer<vtkCleanPolyData> cleanPolyData = 
		vtkSmartPointer<vtkCleanPolyData>::New();
	cleanPolyData->SetInputConnection(reader->GetOutputPort());
	cleanPolyData->Update();

	std::cout<< polydata->GetNumberOfPoints() <<std::endl;
	std::cout<< cleanPolyData->GetOutput()->GetNumberOfPoints()<<std::endl;*/

	//polydata = cleanPolyData->GetOutput();
	
	//initialize label data for store in vtk file
	

	this->polyObject = polydata;

	//see if skeleton vtk
	vtkDataArray* skelVtk = polydata->GetPointData()->GetArray("Radius");
	if(skelVtk == NULL)
		isSkeleton = false;
	else
		isSkeleton = true;

	labelData.clear();
	labelData.resize(polydata->GetPoints()->GetNumberOfPoints());
	for(int i = 0; i < labelData.size(); i++)
		labelData[i] = 0.0;

	// Create a mapper
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
	mapper->SetInput ( polydata );
#else
	mapper->SetInputData ( polydata );
#endif

	// Create an actor
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper ( mapper );

	std::cout << "Actor address: " << actor << std::endl;

	// A renderer and render window
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();

	this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetRenderWindow(this->qvtkWidget->GetRenderWindow());

	//renderWindowInteractor->SetInteractorStyle( style );	
	renderer->AddActor ( actor );
	

	renderer->SetBackground ( 0,0,1 );	
	renderer->ResetCamera();

	// Set the custom style to use for interaction.
	vtkSmartPointer<MouseInteractorAdd> style =
		vtkSmartPointer<MouseInteractorAdd>::New();
	style->qtObject = this;
	style->SetDefaultRenderer(renderer);	

	//reset everything
	vectorTagPoints.clear();
	vectorClassifyPoints.clear();
	vectorTagTriangles.clear();
	vectorTagEdges.clear();

	this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style );
	if(this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer() != NULL)
		this->qvtkWidget->GetRenderWindow()->RemoveRenderer(this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
	this->qvtkWidget->update();	

	this->Connections->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
		vtkCommand::LeftButtonPressEvent,
		this,
		SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));

	//see triangle
	vtkDoubleArray* triDBL = (vtkDoubleArray*)polydata->GetPointData()->GetArray("TagTriangles");
	if(triDBL != NULL)
	{
		readCustomData(polydata);
		std::cout<<"You get triangles ! "<<std::endl;
	}
}