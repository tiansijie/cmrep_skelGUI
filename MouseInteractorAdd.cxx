#include "MouseInteractorAdd.h"	

std::vector<TagInfo> MouseInteractorAdd::vectorTagInfo;
std::vector<TagTriangle> MouseInteractorAdd::vectorTagTriangles;
std::vector<TagPoint> MouseInteractorAdd::vectorTagPoints;
std::vector<std::vector<TagPoint>> MouseInteractorAdd::vectorClassifyPoints;
std::vector<TagEdge> MouseInteractorAdd::vectorTagEdges;
//store all the label info, 0 represent no tag on this point
std::vector<double> MouseInteractorAdd::labelData;
std::vector<vtkActor*> MouseInteractorAdd::triNormalActors;
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

void MouseInteractorAdd::setNormalGenerator(vtkSmartPointer<vtkPolyDataNormals> normalGenerator)
{
	this->normalGenerator = normalGenerator;
}

void MouseInteractorAdd::DrawTriangle()
{
	int id1 = triPtIds[0], id2 = triPtIds[1], id3 = triPtIds[2];
	if(id1 == id2 || id1 == id3 || id2 == id3)
		return;	
	
	vtkSmartPointer<vtkPolyData> normalPolyData = normalGenerator->GetOutput();
	
	vtkFloatArray* normalDataFloat =
		vtkFloatArray::SafeDownCast(normalPolyData->GetPointData()->GetArray("Normals"));
	float result[3];
	if(normalDataFloat)
	{
		int nc = normalDataFloat->GetNumberOfTuples();
		std::cout << "There are " << nc
			<< " components in normalDataFloat" << std::endl;

		float *normal1 = new float[3];
		normalDataFloat->GetTupleValue(vectorTagPoints[id1].seq, normal1);
		float *normal2 = new float[3];
		normalDataFloat->GetTupleValue(vectorTagPoints[id2].seq, normal2);
		float *normal3 = new float[3];
		normalDataFloat->GetTupleValue(vectorTagPoints[id2].seq, normal3);

		float normalAverage[3];
		normalAverage[0] = (normal1[0] + normal2[0] + normal3[0]) / 3.0f;
		normalAverage[1] = (normal1[1] + normal2[1] + normal3[1]) / 3.0f;
		normalAverage[2] = (normal1[2] + normal2[2] + normal3[2]) / 3.0f;

		float d1[3];
		d1[0] = vectorTagPoints[id2].pos[0] - vectorTagPoints[id1].pos[0];
		d1[1] = vectorTagPoints[id2].pos[1] - vectorTagPoints[id1].pos[1];
		d1[2] = vectorTagPoints[id2].pos[2] - vectorTagPoints[id1].pos[2];
		float d2[3];
		d2[0] = vectorTagPoints[id3].pos[0] - vectorTagPoints[id2].pos[0];
		d2[1] = vectorTagPoints[id3].pos[1] - vectorTagPoints[id2].pos[1];
		d2[2] = vectorTagPoints[id3].pos[2] - vectorTagPoints[id2].pos[2];
				
		vtkMath::Cross(d1, d2, result);
		vtkMath::Normalize(result);
		vtkMath::Normalize(normalAverage);

		float cos = vtkMath::Dot(result, normalAverage);

		if(cos < 0){//need to swap
			std::cout<<"Need to swap"<<std::endl;
			int tempid = triPtIds[1];
			triPtIds[1] = triPtIds[2];
			triPtIds[2] = tempid;
			tempid = id2;
			id2 = id3;
			id3 = tempid;
		}		
		std::cout<<"see this result "<<result[0]<<" "<<result[1]<<" "<<result[2]<<std::endl;

		d1[0] = vectorTagPoints[id2].pos[0] - vectorTagPoints[id1].pos[0];
		d1[1] = vectorTagPoints[id2].pos[1] - vectorTagPoints[id1].pos[1];
		d1[2] = vectorTagPoints[id2].pos[2] - vectorTagPoints[id1].pos[2];
		d2[0] = vectorTagPoints[id3].pos[0] - vectorTagPoints[id2].pos[0];
		d2[1] = vectorTagPoints[id3].pos[1] - vectorTagPoints[id2].pos[1];
		d2[2] = vectorTagPoints[id3].pos[2] - vectorTagPoints[id2].pos[2];
		vtkMath::Cross(d1, d2, result);		
		vtkMath::Normalize(result);

		std::cout<<"after see this result "<<result[0]<<" "<<result[1]<<" "<<result[2]<<std::endl;
		std::cout<<"see this normalAverage "<<normalAverage[0]<<" "<<normalAverage[1]<<" "<<normalAverage[2]<<std::endl;
	}
		
	int edgeid1 = PairNumber(triPtIds[0], triPtIds[1]);//id
	int edgeid2 = PairNumber(triPtIds[1], triPtIds[2]);
	int edgeid3 = PairNumber(triPtIds[2], triPtIds[0]);
	std::cout<<"ID "<<id1<<" "<<id2<<" "<<id3<<std::endl;
	//std::cout<<"Pair N "<<edgeid1<<" "<<edgeid2<<" "<<edgeid3<<std::endl;
	int cons1 = ConstrainEdge(vectorTagInfo[vectorTagPoints[id1].comboBoxIndex].tagType, vectorTagInfo[vectorTagPoints[id2].comboBoxIndex].tagType);
	int cons2 = ConstrainEdge(vectorTagInfo[vectorTagPoints[id2].comboBoxIndex].tagType, vectorTagInfo[vectorTagPoints[id3].comboBoxIndex].tagType);
	int cons3 = ConstrainEdge(vectorTagInfo[vectorTagPoints[id1].comboBoxIndex].tagType, vectorTagInfo[vectorTagPoints[id3].comboBoxIndex].tagType);

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

	/*double *n = new double;
	triangle->ComputeNormal(vectorTagPoints[triPtIds[0]].pos, vectorTagPoints[triPtIds[1]].pos, vectorTagPoints[triPtIds[2]].pos, n);
	std::cout<<"triangle n "<<n[0]<<" "<<n[1]<<" "<<n[2]<<std::endl;*/

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
	//mapper->SetInput(appendFilter->GetOutput());
#else
	mapper->SetInputData(trianglePolyData);
#endif
	vtkSmartPointer<vtkActor> actor =
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetEdgeVisibility(true);
	actor->GetProperty()->SetEdgeColor(0.0,0.0,0.0);
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
	tri.seq1 = vectorTagPoints[triPtIds[0]].seq;
	tri.seq2 = vectorTagPoints[triPtIds[1]].seq;
	tri.seq3 = vectorTagPoints[triPtIds[2]].seq;
	vectorTagTriangles.push_back(tri);
	this->GetDefaultRenderer()->AddActor(actor);	

	//draw normal
	double p2[3];
	p2[0] = trianglePolyData->GetCenter()[0] + result[0];
	p2[1] = trianglePolyData->GetCenter()[1] + result[1];
	p2[2] = trianglePolyData->GetCenter()[2] + result[2];

	vtkSmartPointer<vtkLineSource> lineSource = 
		vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(trianglePolyData->GetCenter());
	lineSource->SetPoint2(p2);
	lineSource->Update();

	vtkSmartPointer<vtkSphereSource> sphereEndSource =
		vtkSmartPointer<vtkSphereSource>::New();
	sphereEndSource->SetCenter(p2);
	sphereEndSource->SetRadius(0.3);
	sphereEndSource->Update();

	//Append the two meshes 
	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();

	appendFilter->AddInputConnection(lineSource->GetOutputPort());
	appendFilter->AddInputConnection(sphereEndSource->GetOutputPort());
	appendFilter->Update();

	// Create mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapperNormal =
		vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
	mapperNormal->SetInput(appendFilter->GetOutput());
#else
	mapperNormal->SetInputData(appendFilter->GetOutput());
#endif
	vtkSmartPointer<vtkActor> actorNormal =
		vtkSmartPointer<vtkActor>::New();
	actorNormal->SetMapper(mapperNormal);
	actorNormal->GetProperty()->SetColor(0.3, 0.7, 0.7);

	triNormalActors.push_back(actorNormal);
	this->GetDefaultRenderer()->AddActor(actorNormal);
}

void MouseInteractorAdd::DeleteTriangle(double* pos){

	vtkSmartPointer<vtkActor> pickedActor
		= vtkSmartPointer<vtkActor>::New();

	pickedActor = PickActorFromTriangle(pos);

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
				this->GetDefaultRenderer()->RemoveActor(pickedActor);
				this->GetDefaultRenderer()->RemoveActor(triNormalActors[i]);
				vectorTagTriangles.erase(vectorTagTriangles.begin() + i);
				triNormalActors.erase(triNormalActors.begin() + i);
				i--;
			}
		}
	}
}
	
vtkActor* MouseInteractorAdd::PickActorFromMesh(double pos[3])
{
	vtkSmartPointer<vtkActor> pickedActor
		= vtkSmartPointer<vtkActor>::New();

	for(int i = 0; i < vectorTagPoints.size(); i++)
	{
		if(Distance(vectorTagPoints[i].pos, pos) < 1.0)
		{
			return vectorTagPoints[i].actor;
		}
	}
	return NULL;
}

vtkActor* MouseInteractorAdd::PickActorFromTriangle(double pos[3])
{
	for(int i = 0; i < vectorTagTriangles.size(); i++)
	{
		vtkActor* triActor = vectorTagTriangles[i].triActor;

		vtkSmartPointer<vtkActorCollection> actorCollection 
			= vtkSmartPointer<vtkActorCollection>::New();
		triActor->GetActors(actorCollection);
		vtkTriangle* tri = vtkTriangle::SafeDownCast(actorCollection->GetItemAsObject(0));
		if(tri->PointInTriangle(pos, vectorTagTriangles[i].p1, vectorTagTriangles[i].p2, vectorTagTriangles[i].p3, 0.5))
			return vectorTagTriangles[i].triActor;
	}
	return NULL;
}

void MouseInteractorAdd::FlipNormal(double* pos)
{
	vtkSmartPointer<vtkActor> pickedActor
		= vtkSmartPointer<vtkActor>::New();
	pickedActor = PickActorFromTriangle(pos);
	for(int i = 0; i < vectorTagTriangles.size(); i++)
	{
		if(vectorTagTriangles[i].triActor == pickedActor)
		{
			//flip the 2nd and 3rd vertices 
			int tempChange;
			tempChange = vectorTagTriangles[i].id2;
			vectorTagTriangles[i].id2 = vectorTagTriangles[i].id3;
			vectorTagTriangles[i].id3 = tempChange;

			tempChange = vectorTagTriangles[i].seq2;
			vectorTagTriangles[i].seq2 = vectorTagTriangles[i].seq3;
			vectorTagTriangles[i].seq3 = tempChange;

			double tempPos[3];
			tempPos[0] = vectorTagTriangles[i].p2[0]; tempPos[1] = vectorTagTriangles[i].p2[1]; tempPos[2] = vectorTagTriangles[i].p2[2]; 
			vectorTagTriangles[i].p2[0] = vectorTagTriangles[i].p3[0]; vectorTagTriangles[i].p2[1] = vectorTagTriangles[i].p3[1]; vectorTagTriangles[i].p2[2] = vectorTagTriangles[i].p3[2];
			vectorTagTriangles[i].p3[0] = tempPos[0]; vectorTagTriangles[i].p3[1] = tempPos[1]; vectorTagTriangles[i].p3[2] = tempPos[2];

			//remove the original triangle
			this->GetDefaultRenderer()->RemoveActor(pickedActor);
			this->GetDefaultRenderer()->RemoveActor(triNormalActors[i]);

			//create new triangle
			vtkSmartPointer<vtkPoints> pts =
				vtkSmartPointer<vtkPoints>::New();
			pts->InsertNextPoint(vectorTagTriangles[i].p1);
			pts->InsertNextPoint(vectorTagTriangles[i].p2);
			pts->InsertNextPoint(vectorTagTriangles[i].p3);

			vtkSmartPointer<vtkTriangle> triangle =
				vtkSmartPointer<vtkTriangle>::New();
			triangle->GetPointIds()->SetId ( 0, 0 );
			triangle->GetPointIds()->SetId ( 1, 1 );
			triangle->GetPointIds()->SetId ( 2, 2 );

			double *n = new double[3];
			triangle->ComputeNormal(vectorTagTriangles[i].p1, vectorTagTriangles[i].p2, vectorTagTriangles[i].p3, n);
			std::cout<<"triangle n "<<n[0]<<" "<<n[1]<<" "<<n[2]<<std::endl;

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
			actor->GetProperty()->SetEdgeVisibility(true);
			actor->GetProperty()->SetEdgeColor(0.0,0.0,0.0);
			actor->GetProperty()->SetColor(triCol);

			vectorTagTriangles[i].triActor = actor;

			this->GetDefaultRenderer()->AddActor(actor);

			//draw normal
			double p2[3];
			p2[0] = trianglePolyData->GetCenter()[0] + n[0];
			p2[1] = trianglePolyData->GetCenter()[1] + n[1];
			p2[2] = trianglePolyData->GetCenter()[2] + n[2];
			vtkSmartPointer<vtkLineSource> lineSource = 
				vtkSmartPointer<vtkLineSource>::New();
			lineSource->SetPoint1(trianglePolyData->GetCenter());
			lineSource->SetPoint2(p2);
			lineSource->Update();

			vtkSmartPointer<vtkSphereSource> sphereEndSource =
				vtkSmartPointer<vtkSphereSource>::New();
			sphereEndSource->SetCenter(p2);
			sphereEndSource->SetRadius(0.3);
			sphereEndSource->Update();


			//Append the two meshes 
			vtkSmartPointer<vtkAppendPolyData> appendFilter =
				vtkSmartPointer<vtkAppendPolyData>::New();

			appendFilter->AddInputConnection(lineSource->GetOutputPort());
			appendFilter->AddInputConnection(sphereEndSource->GetOutputPort());
			appendFilter->Update();

			// Create mapper and actor
			vtkSmartPointer<vtkPolyDataMapper> mapperNormal =
				vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION <= 5
			mapperNormal->SetInput(appendFilter->GetOutput());
#else
			mapperNormal->SetInputData(appendFilter->GetOutput());
#endif
			vtkSmartPointer<vtkActor> actorNormal =
				vtkSmartPointer<vtkActor>::New();
			actorNormal->SetMapper(mapperNormal);
			actorNormal->GetProperty()->SetColor(0.3, 0.7, 0.7);

			triNormalActors[i] = actorNormal;
			this->GetDefaultRenderer()->AddActor(actorNormal);
		}
	}
}

void MouseInteractorAdd::AddPoint(double* pos)
{
	//see if there is any tag have been added
	if(vectorTagInfo.size() != 0)
	{
		//find the first actor
		vtkSmartPointer<vtkActorCollection> actors = this->GetDefaultRenderer()->GetActors();
		vtkSmartPointer<vtkActor> actor0 =  static_cast<vtkActor *>(actors->GetItemAsObject(0));	
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

			//QComboBox* cbTagPoint;// =  qtObject->getTagComboBox();
			TagInfo ti = vectorTagInfo[selectedTag];
			vtkSmartPointer<vtkActor> actor =
				vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			actor->GetProperty()->SetColor(ti.tagColor[0] / 255.0, ti.tagColor[1] / 255.0, ti.tagColor[2] / 255.0);
						
			//store actor in vectorTagPoints
			TagPoint actorT;
			actorT.actor = actor;
			actorT.typeIndex = ti.tagIndex;
			actorT.comboBoxIndex = selectedTag;
			//actorT.typeName = cbTagPoint->currentText().toStdString(); 
			actorT.radius = pointRadius;
			actorT.seq = pointSeq;
			actorT.pos[0] = finalPos[0]; actorT.pos[1] = finalPos[1]; actorT.pos[2] = finalPos[2];
			vectorTagPoints.push_back(actorT);

			//calculate the biggest number of edges possibility
			vectorTagEdges.resize(PairNumber(vectorTagPoints.size(), vectorTagPoints.size()));
							
			labelData[pointSeq] = ti.tagIndex;//selectedTag + 1;//cbTagPoint->currentIndex() + 1;
						
			/*if(vectorClassifyPoints.size() <= selectedTag/ *cbTagPoint->currentIndex()* /){
				vectorClassifyPoints.resize(selectedTag/ *cbTagPoint->currentIndex()* / + 1);
			}
			vectorClassifyPoints[selectedTag/ *cbTagPoint->currentIndex()* /].push_back(actorT);*/

			this->GetDefaultRenderer()->AddActor(actor);
		}
	}
}

void MouseInteractorAdd::DeletePoint(double* pos)
{
	vtkSmartPointer<vtkActor> pickedActor
		= vtkSmartPointer<vtkActor>::New();

	pickedActor = PickActorFromMesh(pos);

	//erase from vectorTagPoints and color other points
	for(int i = 0; i < vectorTagPoints.size(); i++){
		TagPoint at = vectorTagPoints[i];
		double* acotrPos = new double[3];
		acotrPos = at.actor->GetPosition();
		if(pickedActor != at.actor){							
			at.actor->GetProperty()->SetColor(vectorTagInfo[at.comboBoxIndex].tagColor[0] / 255.0,
				vectorTagInfo[at.comboBoxIndex].tagColor[1] / 255.0, 
				vectorTagInfo[at.comboBoxIndex].tagColor[2] / 255.0);
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
}

void MouseInteractorAdd::PickPointForTri(double* pos)
{
	vtkSmartPointer<vtkActor> pickedActor
		= vtkSmartPointer<vtkActor>::New();

	pickedActor = PickActorFromMesh(pos);

	if(pickedActor != NULL){
		for(int i = 0; i < vectorTagPoints.size(); i++){
			TagPoint at = vectorTagPoints[i];
			if(pickedActor == at.actor){	
				pickedActor->GetProperty()->SetColor(1,1,1);
				triPtIds.push_back(i);
			}
		}

		if(triPtIds.size() == 3){
			DrawTriangle();
			for(int i = 0; i < triPtIds.size(); i++){
				TagPoint at = vectorTagPoints[triPtIds[i]];
				vectorTagPoints[triPtIds[i]].actor->GetProperty()->SetColor(vectorTagInfo[at.comboBoxIndex].tagColor[0] / 255.0,
					vectorTagInfo[at.comboBoxIndex].tagColor[1] / 255.0, 
					vectorTagInfo[at.comboBoxIndex].tagColor[2] / 255.0);
			}
			triPtIds.resize(0);
		}
	}
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
	std::vector<int> clearEdgeId;
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
			clearEdgeId.push_back(nori);
			clearEdgeId.push_back(d1);
			clearEdgeId.push_back(d2);
			//std::cout<<"0 id"<<nori<<" "<<d1<<" "<<d2<<std::endl;
			this->GetDefaultRenderer()->RemoveActor(vectorTagTriangles[i].triActor);
			this->GetDefaultRenderer()->RemoveActor(triNormalActors[i]);
			triNormalActors.erase(triNormalActors.begin() + i);
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
					clearEdgeId.push_back(nori);
				}
			}
			vectorTagTriangles[i].id1 = deleteEdgeHelper2(id1,seq);
			vectorTagTriangles[i].id2 = deleteEdgeHelper2(id2,seq);
			vectorTagTriangles[i].id3 = deleteEdgeHelper2(id3,seq);
		}			
	}

	for (int i = 0; i < clearEdgeId.size(); i++)
	{
		int id = clearEdgeId[i];
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

void MouseInteractorAdd::reset()
{
	for(int i = 0; i < triPtIds.size(); i++){
		TagPoint at = vectorTagPoints[triPtIds[i]];
		vectorTagPoints[triPtIds[i]].actor->GetProperty()->SetColor(vectorTagInfo[at.comboBoxIndex].tagColor[0] / 255.0,
			vectorTagInfo[at.comboBoxIndex].tagColor[1] / 255.0, 
			vectorTagInfo[at.comboBoxIndex].tagColor[2] / 255.0);
	}
	triPtIds.resize(0);
}

vtkStandardNewMacro(MouseInteractorAdd);
