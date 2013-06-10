#include "EventQtSlotConnect.h"

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


#include <vtkDelaunay3D.h>
#include <sstream> 

// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorStyle2* New();
	vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);

	double posX, posY, posZ;
	struct PickPoint{
		double p[3];
	};
	std::vector<PickPoint>pickPoints;

	virtual void OnLeftButtonDown()
	{
		if( this->Interactor->GetControlKey() ){
			int* clickPos = this->GetInteractor()->GetEventPosition();
			// Pick from this location.
			vtkSmartPointer<vtkPropPicker>  picker =
				vtkSmartPointer<vtkPropPicker>::New();
			picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

			double* pos = picker->GetPickPosition();
			posX = pos[0]; posY = pos[1]; posZ = pos[2];
			//position = picker->->GetPickPosition();
			std::cout << "Pick position (world coordinates) is: "
				<< pos[0] << " " << pos[1]
			<< " " << pos[2] << std::endl;


			std::cout << "Picked actor: " << picker->GetActor() << std::endl;

			

			//find the first actor
			vtkSmartPointer<vtkActorCollection> actors = this->GetCurrentRenderer()->GetActors();
			vtkSmartPointer<vtkActor> actor0 =  static_cast<vtkActor *>(actors->GetItemAsObject(0));

			vtkSmartPointer<vtkDataSet> vtkdata = actor0->GetMapper()->GetInputAsDataSet();
			double minDistance = DBL_MAX;
			double finalPos[3];
			for(vtkIdType i = 0; i < vtkdata->GetNumberOfPoints(); i++){
				double p[3];
				vtkdata->GetPoint(i,p);
				double dist = std::sqrt(std::pow(pos[0] - p[0],2) + std::pow(pos[1] - p[1], 2) + std::pow(pos[2] - p[2], 2));
				if(dist < minDistance){
					minDistance = dist;
					finalPos[0] = p[0]; finalPos[1] = p[1]; finalPos[2] = p[2];
				}
			}
			PickPoint temp;
			temp.p[0] = finalPos[0];temp.p[1] = finalPos[1]; temp.p[2] = finalPos[2];
			pickPoints.push_back(temp);
			std::cout << "Pick position (final position) is: "
				<< finalPos[0] << " " << finalPos[1]
			<< " " << finalPos[2] << std::endl;

			if(pickPoints.size() >= 2){
				vtkSmartPointer<vtkPoints> pts =
					vtkSmartPointer<vtkPoints>::New();
				for(int i = 0; i < pickPoints.size(); i++){
					pts->InsertNextPoint(pickPoints[i].p);
				}
				vtkSmartPointer<vtkPolyLine> polyLine = 
					vtkSmartPointer<vtkPolyLine>::New();
				polyLine->GetPointIds()->SetNumberOfIds(pickPoints.size());
				for(unsigned int i = 0; i < pickPoints.size(); i++){
					polyLine->GetPointIds()->SetId(i,i);
				}
				// Create a cell array to store the lines in and add the lines to it
				vtkSmartPointer<vtkCellArray> cells = 
					vtkSmartPointer<vtkCellArray>::New();
				cells->InsertNextCell(polyLine);

				// Create a polydata to store everything in
				vtkSmartPointer<vtkPolyData> polyData = 
					vtkSmartPointer<vtkPolyData>::New();

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
#endif
				vtkSmartPointer<vtkActor> actor = 
					vtkSmartPointer<vtkActor>::New();
				actor->SetMapper(mapper);
				this->GetDefaultRenderer()->AddActor(actor);
			}


			//Create a sphere
			vtkSmartPointer<vtkSphereSource> sphereSource =
				vtkSmartPointer<vtkSphereSource>::New();
			sphereSource->SetCenter(finalPos[0], finalPos[1], finalPos[2]);
			sphereSource->SetRadius(0.2);

			//Create a mapper and actor
			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(sphereSource->GetOutputPort());

			vtkSmartPointer<vtkActor> actor =
				vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

			this->GetDefaultRenderer()->AddActor(actor);
			// Forward events
		
		}
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
	}
private:
};

vtkStandardNewMacro(MouseInteractorStyle2);

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

  this->connect(&this->FutureWatcher, SIGNAL(finished()), this, SLOT(slot_finished()));
  this->connect(this->cmrepVskel, SIGNAL(clicked()), this, SLOT(executeCmrepVskel()));
};

void EventQtSlotConnect::slot_finished(){
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

void EventQtSlotConnect::open(){
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
	if (!fileName.isEmpty()) {
		VTKfilename = fileName.toStdString();
		readVTK(VTKfilename);
	}
}

void EventQtSlotConnect::save(){

}

void EventQtSlotConnect::executeCmrepVskel(){

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

/*
	std::cout<<"parameter"<<std::endl;
	for(int i = 0; i < parameters.size(); i++)
		std::cout<<parameters[i]<<std::endl;*/
	this->cmrep_progressBar->show();
	QFuture<void> future = QtConcurrent::run(&this->v, &VoronoiSkeletonTool::execute, parameters.size(), parameters);
	this->FutureWatcher.setFuture(future);
	/*this->ProgressDialog->setMinimum(0);
	this->ProgressDialog->setMaximum(0);
	this->ProgressDialog->setWindowModality(Qt::WindowModal);
	this->ProgressDialog->exec();*/
	//v.execute(parameters.size(), parameters);

	VTKfilename = outputNameSkel;
}

void EventQtSlotConnect::createActions(){
	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
}

void EventQtSlotConnect::createMenus(){
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
	menuBar()->addMenu(fileMenu);
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


	vtkIdType;
	std::cout<< polydata->GetNumberOfPoints() <<std::endl;
  
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
//	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//		vtkSmartPointer<vtkRenderWindowInteractor>::New();
//	renderWindowInteractor->SetRenderWindow( this->qvtkWidget->GetRenderWindow());

	this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetRenderWindow(this->qvtkWidget->GetRenderWindow());

	//renderWindowInteractor->SetInteractorStyle( style );	
	renderer->AddActor ( actor );
	renderer->SetBackground ( 0,0,1 );	
	renderer->ResetCamera();


	// Set the custom stype to use for interaction.
	vtkSmartPointer<MouseInteractorStyle2> style =
		vtkSmartPointer<MouseInteractorStyle2>::New();
	style->SetDefaultRenderer(renderer);
	

	this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style );
	this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
	this->qvtkWidget->update();
	

	this->Connections->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
		vtkCommand::LeftButtonPressEvent,
		this,
		SLOT(slot_clicked(vtkObject*, unsigned long, void*, void*)));
}