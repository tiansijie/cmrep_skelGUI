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
	VoronoiSkeletonTool v;
	char *command[3];
	command[0] = "cmreop_vskel";
	command[1] = new char[VTKfilename.length() + 1];
	strcpy(command[1], VTKfilename.c_str());

	std::string outputNameSkel =  VTKfilename;
	outputNameSkel = outputNameSkel.substr(0, outputNameSkel.length() - 4) + "_Skel.vtk";
	command[2] = new char [outputNameSkel.length() + 1];
	strcpy(command[2], outputNameSkel.c_str());

	v.execute(3, command);
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