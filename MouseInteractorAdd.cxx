#include "MouseInteractorAdd.h"	

std::vector<TagInfo> MouseInteractorAdd::vectorTagInfo;
std::vector<TagTriangle> MouseInteractorAdd::vectorTagTriangles;
std::vector<TagPoint> MouseInteractorAdd::vectorTagPoints;
std::vector<std::vector<TagPoint>> MouseInteractorAdd::vectorClassifyPoints;
std::vector<TagEdge> MouseInteractorAdd::vectorTagEdges;
//store all the label info, 0 represent no tag on this point
std::vector<double> MouseInteractorAdd::labelData;
bool MouseInteractorAdd::isSkeleton;
int MouseInteractorAdd::selectedTag;

MouseInteractorAdd::MouseInteractorAdd(){
	selectedTriangle = NULL;
//	qtObject = NULL;
	triCol[0] = 0.2; triCol[1] = 0.2; triCol[2] = 0.7;
}

double MouseInteractorAdd::Distance(double p1[3], double p2[3]){
	return std::sqrt(std::pow(p1[0] - p2[0],2) + std::pow(p1[1] - p2[1], 2) + std::pow(p1[2] - p2[2], 2));
}

void MouseInteractorAdd::SetSelectedTriColor(){
	for(int i = 0; i < vectorTagTriangles.size(); i++){
		if(selectedTriangle == vectorTagTriangles[i].triActor)
			selectedTriangle->GetProperty()->SetColor(1,1,0);
		else
			vectorTagTriangles[i].triActor->GetProperty()->SetColor(triCol);
	}
}

void MouseInteractorAdd::DrawDelaunayTriangle(){
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

void MouseInteractorAdd::DrawRegularTriangle(){
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

int MouseInteractorAdd::ConstrainEdge(int type1, int type2)
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

int MouseInteractorAdd::PairNumber(int a, int b){
	int a1 = std::min(a,b);
	int b1 = std::max(a,b);
	return (a1 + b1) * (a1 + b1 + 1) / 2.0 + b1;
}

void MouseInteractorAdd::DrawTriangle()
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

void MouseInteractorAdd::DeleteTriangle(vtkActor* pickedActor){
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
	
vtkActor* MouseInteractorAdd::PickAcotrFromMesh(double pos[3])
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

void MouseInteractorAdd::copyEdgeBtoA(int a, int b){
	vectorTagEdges[a].constrain = vectorTagEdges[b].constrain;
	vectorTagEdges[a].numEdge = vectorTagEdges[b].numEdge;
}

int MouseInteractorAdd::deleteEdgeHelper(int id1, int id2, int seq)
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

int MouseInteractorAdd::deleteEdgeHelper2(int id, int seq)
{
	if(id > seq)
		return id - 1;
	else
		return id;
}

void MouseInteractorAdd::deleteEdge(int seq)
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

vtkStandardNewMacro(MouseInteractorAdd);
