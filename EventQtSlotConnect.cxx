#include "EventQtSlotConnect.h"

#include "VoronoiSkeletonTool.h"

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

#include <QtGui>

#include <vtkDelaunay3D.h>
#include <sstream> 




// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorStyle2* New();
	vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);

	double posX, posY, posZ;

	virtual void OnLeftButtonDown()
	{
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

		if( this->GetInteractor()->GetControlKey() ){
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

			//Create a sphere
			vtkSmartPointer<vtkSphereSource> sphereSource =
				vtkSmartPointer<vtkSphereSource>::New();
			sphereSource->SetCenter(pos[0], pos[1], pos[2]);
			sphereSource->SetRadius(1.0);

			//Create a mapper and actor
			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(sphereSource->GetOutputPort());

			vtkSmartPointer<vtkActor> actor =
				vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

			//this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetDefaultRenderer()->AddActor(actor);
			this->GetDefaultRenderer()->AddActor(actor);
			// Forward events
		
		}
	}
private:

};

vtkStandardNewMacro(MouseInteractorStyle2);

// Constructor
EventQtSlotConnect::EventQtSlotConnect()
{
  this->setupUi(this);

  this->Connections = vtkSmartPointer<vtkEventQtSlotConnect>::New();

  createActions();
  createMenus();

  this->connect(this->cmrepVskel, SIGNAL(clicked()), this, SLOT(executeCmrepVskel()));

/////////////////////////////////////////////////////
/*  // Sphere
  vtkSmartPointer<vtkSphereSource> sphereSource =
    vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkSmartPointer<vtkActor> sphereActor =
    vtkSmartPointer<vtkActor>::New();
  sphereActor->SetMapper(sphereMapper);

  // VTK Renderer
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(sphereActor);*/
/////////////////////////////////////////////////////


//vtkSmartPointer<vtkPlaneSource> planeSource =
//		vtkSmartPointer<vtkPlaneSource>::New();
//	planeSource->Update();
};


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

	//textEdit;
	//std::cout<<"eparameter is " << this->eParameter->value()<< std::endl;
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
	//command[0] = "cmreop_vskel";
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
	VoronoiSkeletonTool v;
	v.execute(parameters.size(), parameters);
	readVTK(outputNameSkel);
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