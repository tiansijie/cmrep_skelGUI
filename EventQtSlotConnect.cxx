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

#include <vtkDelaunay3D.h>
#include <vtkDelaunay2D.h>
#include <vtkShrinkFilter.h>
#include <vtkTriangle.h>
#include <vtkPointData.h>
#include <vtkGenericDataObjectWriter.h>
#include <sstream>


double pointColor[3];

struct TagPoint{
	vtkActor* actor;
	std::string typeName;
	int typeIndex;
	double radius;//radius of that points
};	
std::vector<TagPoint> vectorTagPoints;
std::vector<std::vector<TagPoint>> vectorClassifyPoints;


struct TagInfo
{
	std::string tagName;
	int tagType; // 1 = Branch point  2 = Free Edge point 3 = Interior point  4 = others
	double tagColor[3];
};
std::vector<TagInfo> vectorTagInfo;
//std::vector<std::string> vectorTagNames;

bool isSkeleton;

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

	double posX, posY, posZ;
	struct PickPoint{
		double p[3];
	};
	std::vector<PickPoint>pickPoints;	

	struct TriangleInfo{
		vtkActor *triActor;
		double p1[3], p2[3], p3[3];
		double centerPos[3];
	};
	std::vector<TriangleInfo>VectorTriangles;

	vtkActor *selectedTriangle;
	vtkActor *prePolyLineActor;

	double Distance(double p1[3], double p2[3]){
		return std::sqrt(std::pow(p1[0] - p2[0],2) + std::pow(p1[1] - p2[1], 2) + std::pow(p1[2] - p2[2], 2));
	}

	void SetSelectedTriColor(){
		for(int i = 0; i < VectorTriangles.size(); i++){
			if(selectedTriangle == VectorTriangles[i].triActor)
				selectedTriangle->GetProperty()->SetColor(1,1,0);
			else
				VectorTriangles[i].triActor->GetProperty()->SetColor(triCol);
		}
	}

	void DrawDelaunayTriangle(){
		if(pickPoints.size() > 3){
			vtkSmartPointer<vtkPoints> pts =
				vtkSmartPointer<vtkPoints>::New();
			for(int i = 0; i < pickPoints.size(); i++){
				pts->InsertNextPoint(pickPoints[i].p);
			}
			/*vtkSmartPointer<vtkPolyLine> polyLine = 
				vtkSmartPointer<vtkPolyLine>::New();
			polyLine->GetPointIds()->SetNumberOfIds(pickPoints.size());
			for(unsigned int i = 0; i < pickPoints.size(); i++){
				polyLine->GetPointIds()->SetId(i,i);
			}
			// Create a cell array to store the lines in and add the lines to it
			vtkSmartPointer<vtkCellArray> cells = 
				vtkSmartPointer<vtkCellArray>::New();
			cells->InsertNextCell(polyLine);	*/		

			// Create a polydata to store everything in
			vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
			polyData->SetPoints(pts);

			vtkSmartPointer<vtkDelaunay3D> delaunay =
				vtkSmartPointer<vtkDelaunay3D>::New();
			#if VTK_MAJOR_VERSION <= 5
			delaunay->SetInput(polyData);
			#else
			delaunay->SetInputData(polyData);
			#endif
			delaunay->Update();

			delaunay->SetAlpha(20);
			//delaunay->SetTolerance(10);
			//delaunay->SetProjectionPlaneMode(2);
			// Visualize
			vtkSmartPointer<vtkShrinkFilter> shrink = 
				vtkSmartPointer<vtkShrinkFilter>::New();
			shrink->SetInputConnection(delaunay->GetOutputPort());
			shrink->SetShrinkFactor(0.9);

			vtkSmartPointer<vtkDataSetMapper> meshMapper =
				vtkSmartPointer<vtkDataSetMapper>::New();
			meshMapper->SetInputConnection(shrink->GetOutputPort());

			vtkSmartPointer<vtkActor> meshActor =
				vtkSmartPointer<vtkActor>::New();
			meshActor->SetMapper(meshMapper);
			meshActor->GetProperty()->SetColor(0.2,0.8,0.5);

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
			vtkSmartPointer<vtkActor> actor = 
				vtkSmartPointer<vtkActor>::New();
			if(prePolyLineActor != NULL)
				this->GetDefaultRenderer()->RemoveActor(prePolyLineActor);
			prePolyLineActor = meshActor;
			//actor->SetMapper(pointMapper);
			//actor->GetProperty()->SetColor(0,1,1);
			//this->GetDefaultRenderer()->AddActor(actor);

			this->GetDefaultRenderer()->AddActor(meshActor);
		}
		else{
			if(prePolyLineActor != NULL)
				this->GetDefaultRenderer()->RemoveActor(prePolyLineActor);
		}
	}

	void DrawRegularTriangle(){
		if(pickPoints.size() > 2){
			double lastPos[3];
			lastPos[0] = pickPoints[pickPoints.size()-1].p[0];
			lastPos[1] = pickPoints[pickPoints.size()-1].p[1];
			lastPos[2] = pickPoints[pickPoints.size()-1].p[2];
			//find two minmal distance;
			double minDis1,minDis2; minDis1 = minDis2 = DBL_MAX;
			double minP1[3], minP2[3];

			if(selectedTriangle == NULL){
				for(int i = 0; i < pickPoints.size() - 1; i++){
					double dis = Distance(pickPoints[i].p, lastPos);
					if(dis < minDis1){
						minDis1 = dis;
						minP1[0] = pickPoints[i].p[0]; minP1[1] = pickPoints[i].p[1]; minP1[2] = pickPoints[i].p[2];
					}				
				}

				for(int i = 0; i < pickPoints.size() - 1; i++){
					double dis = Distance(pickPoints[i].p, lastPos);
					if(dis < minDis2 && dis > minDis1){
						minDis2 = dis;
						minP2[0] = pickPoints[i].p[0]; minP2[1] = pickPoints[i].p[1]; minP2[2] = pickPoints[i].p[2];
					}
				}
			}
			else{
				for(int i = 0; i < VectorTriangles.size(); i++){
					if(VectorTriangles[i].triActor == selectedTriangle){
						double dis1 = Distance(VectorTriangles[i].p1, lastPos);
						double dis2 = Distance(VectorTriangles[i].p2, lastPos);
						double dis3 = Distance(VectorTriangles[i].p3, lastPos);
						minDis1 = std::min(dis1, std::min(dis2, dis3));
						//find minimum two points
						if(dis1 == minDis1){
							minDis2 = std::min(dis2,dis3);
							minP1[0] = VectorTriangles[i].p1[0];minP1[1] = VectorTriangles[i].p1[1];minP1[2] = VectorTriangles[i].p1[2];
							if(minDis2 == dis2){
								minP2[0] = VectorTriangles[i].p2[0];minP2[1] = VectorTriangles[i].p2[1];minP2[2] = VectorTriangles[i].p2[2];
							}
							else if(minDis2 == dis3){
								minP2[0] = VectorTriangles[i].p3[0];minP2[1] = VectorTriangles[i].p3[1];minP2[2] = VectorTriangles[i].p3[2];
							}
						}
						else if(dis2 == minDis1){
							minDis2 = std::min(dis1,dis3);
							minP1[0] = VectorTriangles[i].p2[0];minP1[1] = VectorTriangles[i].p2[1];minP1[2] = VectorTriangles[i].p2[2];
							if(minDis2 == dis1){
								minP2[0] = VectorTriangles[i].p1[0];minP2[1] = VectorTriangles[i].p1[1];minP2[2] = VectorTriangles[i].p1[2];
							}
							else{
								minP2[0] = VectorTriangles[i].p3[0];minP2[1] = VectorTriangles[i].p3[1];minP2[2] = VectorTriangles[i].p3[2];
							}
						}
						else if(dis3 == minDis1){
							minDis2 = std::min(dis1, dis2);
							minP1[0] = VectorTriangles[i].p3[0];minP1[1] = VectorTriangles[i].p3[1];minP1[2] = VectorTriangles[i].p3[2];
							if(minDis2 == dis1){
								minP2[0] = VectorTriangles[i].p1[0];minP2[1] = VectorTriangles[i].p1[1];minP2[2] = VectorTriangles[i].p1[2];
							}
							else{
								minP2[0] = VectorTriangles[i].p2[0];minP2[1] = VectorTriangles[i].p2[1];minP2[2] = VectorTriangles[i].p2[2];
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
			TriangleInfo tri;
			tri.triActor = actor;
			tri.p1[0] = lastPos[0]; tri.p1[1] = lastPos[1]; tri.p1[2] = lastPos[2];
			tri.p2[0] = minP1[0]; tri.p2[1] = minP1[1]; tri.p2[2] = minP1[2];
			tri.p3[0] = minP2[0]; tri.p3[1] = minP2[1]; tri.p3[2] = minP2[2];
			tri.centerPos[0] = trianglePolyData->GetCenter()[0];
			tri.centerPos[1] = trianglePolyData->GetCenter()[1];
			tri.centerPos[2] = trianglePolyData->GetCenter()[2];
			VectorTriangles.push_back(tri);

			selectedTriangle = actor;
			SetSelectedTriColor();
			this->GetDefaultRenderer()->AddActor(actor);
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
			posX = pos[0]; posY = pos[1]; posZ = pos[2];				
			 
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
					if(vectorTagInfo.size() != 0)
					{
						vtkSmartPointer<vtkDataSet> vtkdata = actor0->GetMapper()->GetInputAsDataSet();
						//vtkdata->GetPointData()->GetAbstractArray("Radius");
						vtkDataArray* radiusArray = vtkdata->GetPointData()->GetArray("Radius");
						double minDistance = DBL_MAX;
						double finalPos[3];
						double *pointRadius;
						//finalPos[0] = pos[0]; finalPos[1] = pos[1]; finalPos[2] = pos[2];
						for(vtkIdType i = 0; i < vtkdata->GetNumberOfPoints(); i++){
							double p[3];
							vtkdata->GetPoint(i,p);
							double dist = std::sqrt(std::pow(pos[0] - p[0],2) + std::pow(pos[1] - p[1], 2) + std::pow(pos[2] - p[2], 2));
							if(dist < minDistance){
								minDistance = dist;
								finalPos[0] = p[0]; finalPos[1] = p[1]; finalPos[2] = p[2];
								pointRadius = radiusArray->GetTuple(i);
							}
						}
						PickPoint temp;
						temp.p[0] = finalPos[0];temp.p[1] = finalPos[1]; temp.p[2] = finalPos[2];
						pickPoints.push_back(temp);
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
						actor->GetProperty()->SetColor(ti.tagColor[0], ti.tagColor[1], ti.tagColor[2]);
						
						//store actor in vectorTagPoints
						TagPoint actorT;
						actorT.actor = actor;
						actorT.typeIndex = cbTagPoint->currentIndex();
						actorT.typeName = cbTagPoint->currentText().toStdString(); 
						actorT.radius = pointRadius[0];
						vectorTagPoints.push_back(actorT);
						
						if(vectorClassifyPoints.size() <= cbTagPoint->currentIndex()){
							vectorClassifyPoints.resize(cbTagPoint->currentIndex() + 1);
						}
						vectorClassifyPoints[cbTagPoint->currentIndex()].push_back(actorT);

						this->GetDefaultRenderer()->AddActor(actor);
					}
				}//end of control

				//for delete point event
				else if(key.compare("s") == 0)
				{
					std::cout<<"are you in"<<std::endl;
					vtkSmartPointer<vtkActor> pickedActor
						= vtkSmartPointer<vtkActor>::New();

					vtkActorCollection* actorsCollection = this->GetDefaultRenderer()->GetActors();
					actorsCollection->InitTraversal();
					
					std::cout<<"number of actors: "<<actorsCollection->GetNumberOfItems()<<std::endl;

					for(vtkIdType i = 0; i < actorsCollection->GetNumberOfItems(); i++)
					{
						vtkActor* nextActor = actorsCollection->GetNextActor();
						double* actorPos = nextActor->GetCenter();
						double dis = std::sqrt(std::pow(pos[0] - actorPos[0],2) 
							+ std::pow(pos[1] - actorPos[1], 2) 
							+ std::pow(pos[2] - actorPos[2], 2));
						if(dis < 1.0 && nextActor){
							pickedActor = nextActor;
							break;
						}
					}

					for(int i = 0; i < vectorTagPoints.size(); i++){
						TagPoint at = vectorTagPoints[i];
						double* acotrPos = at.actor->GetPosition();
						if(pickedActor != at.actor){							
							at.actor->GetProperty()->SetColor(vectorTagInfo[at.typeIndex].tagColor[0],
								vectorTagInfo[at.typeIndex].tagColor[1], 
								vectorTagInfo[at.typeIndex].tagColor[2]);
						}					
						else{
							pickPoints.erase(pickPoints.begin() + i);
							for(int j = 0; j < vectorClassifyPoints[vectorTagPoints[i].typeIndex].size(); j++){
								if(vectorClassifyPoints[vectorTagPoints[i].typeIndex][j].actor == at.actor){
									vectorClassifyPoints[vectorTagPoints[i].typeIndex].erase(vectorClassifyPoints[vectorTagPoints[i].typeIndex].begin()
										+ j);
								}
							}
							vectorTagPoints.erase(vectorTagPoints.begin() + i);
							this->GetDefaultRenderer()->RemoveActor(pickedActor);
							//DrawDelaunayTriangle();
							//DrawRegularTriangle();
							i--;
						}
					}
					
				}//end of s

				else if(key.compare("t") == 0)
				{
					std::cout<<"Tri in"<<std::endl;
					vtkActor *actor = picker->GetActor();
					vtkActorCollection* acotrCollection = this->GetDefaultRenderer()->GetActors();
					acotrCollection->InitTraversal();
					
					if(actor != acotrCollection->GetNextActor()){//except for the skeleton
						for(int i = 0; i < VectorTriangles.size(); i++){
							if(actor == VectorTriangles[i].triActor){
								selectedTriangle = actor;
								selectedTriangle->GetProperty()->SetColor(1,1,0);
							}
							else{
								VectorTriangles[i].triActor->GetProperty()->SetColor(triCol);
							}
						}												
					}
				}//end of t
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
	if(addDialog.exec()){
		QString tagText = addDialog.lineEdit->text();

		TagInfo ti;
		ti.tagName = tagText.toStdString();
		ti.tagColor[0] = ((double) std::rand() / (RAND_MAX)); ti.tagColor[1] = ((double) std::rand() / (RAND_MAX)); ti.tagColor[2] = ((double) std::rand() / (RAND_MAX));
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
		pix.fill(QColor(ti.tagColor[0] * 255, ti.tagColor[1] * 255, ti.tagColor[2] * 255));
		this->comboBoxTagPoint->addItem(pix, tagText);
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
  textEdit->append("Clicked");
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
		tr("Save VTK Files"),
		tr("VTK Files (*.vtk)"),
		&selectedFilter,
		options);
	

	vtkSmartPointer<vtkGenericDataObjectWriter> writer = 
		vtkSmartPointer<vtkGenericDataObjectWriter>::New();
	if (!fileName.isEmpty())
		writer->SetFileName(fileName.toStdString().c_str());

	for(int i = 0; i < vectorTagInfo.size(); i++)
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
	}

	writer->SetInput(polyObject);
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

	this->polyObject = polydata;

	//see if skeleton vtk
	vtkDataArray* skelVtk = polydata->GetPointData()->GetArray("Radius");
	if(skelVtk == NULL)
		isSkeleton = false;
	else
		isSkeleton = true;

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

	this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style );
	if(this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer() != NULL)
		this->qvtkWidget->GetRenderWindow()->RemoveRenderer(this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
	this->qvtkWidget->update();	

	this->Connections->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
		vtkCommand::LeftButtonPressEvent,
		this,
		SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));
}