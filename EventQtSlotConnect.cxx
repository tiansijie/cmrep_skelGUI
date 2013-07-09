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
#include <vtkFloatArray.h>
#include <vtkIntArray.h>
#include <vtkStringArray.h>
#include <algorithm>
#include <fstream>

double pointColor[3];

// Constructor
EventQtSlotConnect::EventQtSlotConnect()
{
  this->setupUi(this);

  this->Connections = vtkSmartPointer<vtkEventQtSlotConnect>::New();

  this->cmrep_progressBar->setMinimum(0);
  this->cmrep_progressBar->setMaximum(0);
  this->cmrep_progressBar->hide();

  this->GridTypeComboBox->addItem("Loop Subdivision");
  
  this->SolverTypeComboBox->addItem("Brute Force");
  this->SolverTypeComboBox->addItem("PDE");

  if(this->GridTypeComboBox->currentIndex() == 0)
	  this->SubLevelComboBox->setEnabled(true);

  if(this->SolverTypeComboBox->currentIndex() == 1)
	  this->RhoLineEdit->setEnabled(true);

  if(this->ConsRadiusCheckBox->isChecked())
	  this->RadiusLineEdit->setEnabled(true);

  createActions();
  createMenus();

  pointColor[0] = 1; pointColor[1] = 0; pointColor[2] = 0;

  this->connect(&this->FutureWatcher, SIGNAL(finished()), this, SLOT(slot_finished()));
  this->connect(this->cmrepVskel, SIGNAL(clicked()), this, SLOT(executeCmrepVskel()));

  //Mesh interaction
  this->connect(this->checkBoxHideSkel, SIGNAL(stateChanged(int)), this, SLOT(slot_skelStateChange(int)));
  this->connect(this->checkBoxHideMesh, SIGNAL(stateChanged(int)), this, SLOT(slot_meshStateChange(int)));
  
  //Tag modification
  this->connect(this->pushButtonAddTag, SIGNAL(clicked()), this, SLOT(slot_addTag()));
  this->connect(this->comboBoxTagPoint, SIGNAL(activated(int)), this, SLOT(slot_comboxChanged(int)));
  this->connect(this->pushButtonDeleteTag, SIGNAL(clicked()), this, SLOT(slot_delTag()));
  this->connect(this->pushButtonEditTag, SIGNAL(clicked()), this, SLOT(slot_editTag()));
  
  //Saving option
  this->connect(this->GridTypeComboBox, SIGNAL(activated(int)), this, SLOT(slot_gridTypeChanged(int)));
  this->connect(this->SolverTypeComboBox, SIGNAL(activated(int)), this, SLOT(slot_solverTypeChanged(int)));
  this->connect(this->ConsRadiusCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slot_consRadiusCheck(int)));

  //Decimation
  this->connect(this->TargetReductSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_targetReductSilder(int)));
  this->connect(this->TargetReductLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slot_targetReductEditor(QString)));
  this->connect(this->FeatureAngleSlider, SIGNAL(valueChanged(int)), this, SLOT(slot_featureAngleSlider(int)));
  this->connect(this->FeatureAngleLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slot_feartureAngleEditor(QString)));
  this->connect(this->ApplyDecimateButton, SIGNAL(clicked()), this, SLOT(slot_decimateButton()));
};

void EventQtSlotConnect::slot_addTag(){

	AddTagDialog addDialog;
	addDialog.show();

	/*for(int i = 0; i < MouseInteractorAdd::vectorTagInfo.size(); i++){
		addDialog.setTagName(MouseInteractorAdd::vectorTagInfo[i].tagName);
		addDialog.setTagColor(MouseInteractorAdd::vectorTagInfo[i].qc);
	}*/

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
		ti.tagIndex = addDialog.tagIndex;

		MouseInteractorAdd::vectorTagInfo.push_back(ti);

		QPixmap pix(22,22);
		QString displayText = QString::number(ti.tagIndex) + " " + tagText;
		pix.fill(addDialog.color);
		this->comboBoxTagPoint->addItem(pix, displayText);
	}	
}

void EventQtSlotConnect::slot_delTag()
{
	int curIndex = this->comboBoxTagPoint->currentIndex();
	for(int i = 0; i < MouseInteractorAdd::vectorTagPoints.size(); i++)
	{
		if(curIndex == MouseInteractorAdd::vectorTagPoints[i].comboBoxIndex)
		{
			QMessageBox messageBox;
			messageBox.critical(0,"Error","You need to delete these points in skeleton before deletion");
			return;
		}
	}
	switch( QMessageBox::information( this, "Delete Tag",
		"Are you sure to delete this tag? ",
		"Yes", "Cancel",
		0, 1 ) ) {
	case 0:
		MouseInteractorAdd::vectorTagInfo.erase(MouseInteractorAdd::vectorTagInfo.begin() + curIndex);
		this->comboBoxTagPoint->removeItem(curIndex);
		//new index
		MouseInteractorAdd::selectedTag = this->comboBoxTagPoint->currentIndex();
		break;
	case 1:
	default:
		break;
	}	
}

void EventQtSlotConnect::slot_editTag()
{
	AddTagDialog addDialog;

	TagInfo tio = MouseInteractorAdd::vectorTagInfo[comboBoxTagPoint->currentIndex()];

	addDialog.lineEdit->setText(QString::fromStdString(tio.tagName));
	addDialog.color = tio.qc;
	addDialog.colorLabel->setPalette(QPalette(tio.qc));
	addDialog.colorLabel->setAutoFillBackground(true);
	if(tio.tagType == 1)
		addDialog.branchButton->setChecked(true);
	else if(tio.tagType == 2)
		addDialog.freeEdgeButton->setChecked(true);
	else if(tio.tagType == 3)
		addDialog.interiorButton->setChecked(true);
	else
		addDialog.otherButton->setChecked(true);

	addDialog.indexBox->setCurrentIndex(tio.tagIndex-1);
	addDialog.tagIndex = tio.tagIndex;

	addDialog.show();
	
	if(addDialog.exec())
	{
		QString tagText = addDialog.lineEdit->text();

		TagInfo ti;
		ti.tagName = tagText.toStdString();
		ti.qc = addDialog.color;
		int r, g, b;
		r = addDialog.color.red();
		g = addDialog.color.green();
		b = addDialog.color.blue();
		ti.tagColor[0] = r; ti.tagColor[1] = g; ti.tagColor[2] = b;
		if(addDialog.branchButton->isChecked())
			ti.tagType = 1;
		else if(addDialog.freeEdgeButton->isChecked())
			ti.tagType = 2;
		else if(addDialog.interiorButton->isChecked())
			ti.tagType = 3;
		else
			ti.tagType = 4;
		ti.tagIndex = addDialog.tagIndex;

		MouseInteractorAdd::vectorTagInfo[this->comboBoxTagPoint->currentIndex()] = ti;

		//update the tag point on skeleton
		for(int i = 0; i < MouseInteractorAdd::vectorTagPoints.size(); i++)
		{
			if(MouseInteractorAdd::vectorTagPoints[i].comboBoxIndex == this->comboBoxTagPoint->currentIndex())
			{
				MouseInteractorAdd::vectorTagPoints[i].typeIndex = ti.tagType;
				MouseInteractorAdd::vectorTagPoints[i].typeName = ti.tagName;
				MouseInteractorAdd::vectorTagPoints[i].actor->GetProperty()->SetColor(ti.tagColor[0] / 255.0, ti.tagColor[1] / 255.0, ti.tagColor[2] / 255.0);
			}
		}

		//update combobox
		QPixmap pix(22,22);
		QString displayText = QString::number(ti.tagIndex) + " " + tagText;
		pix.fill(addDialog.color);
		this->comboBoxTagPoint->removeItem(this->comboBoxTagPoint->currentIndex());
		this->comboBoxTagPoint->insertItem(this->comboBoxTagPoint->currentIndex(), pix, displayText);
	}
}

void EventQtSlotConnect::slot_finished()
{
	readVTK(VTKfilename);
	this->cmrep_progressBar->hide();
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
	

	if (!fileName.isEmpty()){	
		saveVTKFile(fileName);
		saveParaViewFile(fileName);
		saveCmrepFile(fileName);	
	}
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

void EventQtSlotConnect::slot_meshStateChange(int state){
	if(state == Qt::Unchecked)
	{
		for(int i = 0; i < MouseInteractorAdd::vectorTagTriangles.size(); i++)
		{
			MouseInteractorAdd::vectorTagTriangles[i].triActor->VisibilityOn();
		}
	}
	else
	{
		for(int i = 0; i < MouseInteractorAdd::vectorTagTriangles.size(); i++)
		{
			MouseInteractorAdd::vectorTagTriangles[i].triActor->VisibilityOff();
		}
	}
	this->qvtkWidget->GetRenderWindow()->Render();
}

void EventQtSlotConnect::slot_comboxChanged(int state)
{
	MouseInteractorAdd::selectedTag = this->comboBoxTagPoint->currentIndex();
}

void EventQtSlotConnect::slot_gridTypeChanged(int state)
{
	if(state == 0)
		this->SubLevelComboBox->setEnabled(true);
	else
		this->SubLevelComboBox->setEnabled(false);
}

void EventQtSlotConnect::slot_solverTypeChanged(int state)
{
	if(state == 1)
		this->RhoLineEdit->setEnabled(true);
	else
		this->RhoLineEdit->setEnabled(false);
}

void EventQtSlotConnect::slot_consRadiusCheck(int state)
{
	if(state == Qt::Checked)
		this->RadiusLineEdit->setEnabled(true);
	else
		this->RadiusLineEdit->setEnabled(false);
}

void EventQtSlotConnect::slot_targetReductSilder(int value)
{
	this->TargetReductLineEdit->setText(QString::number(value/100.0));
}

void EventQtSlotConnect::slot_targetReductEditor(QString text)
{
	this->TargetReductSlider->setValue(text.toDouble() * 100);
}

void EventQtSlotConnect::slot_featureAngleSlider(int value)
{
	this->FeatureAngleLineEdit->setText(QString::number(value));
}

void EventQtSlotConnect::slot_feartureAngleEditor(QString text)
{
	this->FeatureAngleSlider->setValue(text.toDouble());
}

void EventQtSlotConnect::slot_decimateButton()
{
	targetReduction = this->TargetReductLineEdit->text().toDouble();
	featureAngle = this->FeatureAngleLineEdit->text().toDouble();
	Decimate();
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
		//delete temp;
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
		//delete temp;
	}

	int cvalue = this->cParameter->value();
	if(cvalue != 0){
		parameters.push_back("-c");
		char *temp = new char;
		itoa(cvalue, temp, 10);
		parameters.push_back(temp);
		//delete temp;
	}

	int tvalue = this->tParameter->value();
	if(tvalue != 0){
		parameters.push_back("-t");
		char *temp = new char;
		itoa(tvalue, temp, 10);
		parameters.push_back(temp);
		//delete temp;
	}

	QString stextQ = this->sParameter->text();
	std::string stext = stextQ.toStdString();
	if(!stext.empty()){
		parameters.push_back("-s");
		char *temp = new char;
		strcpy(temp, stext.c_str());
		parameters.push_back(temp);
		//delete temp;
	}

	QString RtextQ = this->RParameter->text();
	std::string Rtext = RtextQ.toStdString();
	if(!Rtext.empty()){
		parameters.push_back("-R");
		char *temp = new char;
		strcpy(temp, Rtext.c_str());
		parameters.push_back(temp);
		//delete temp;
	}

	QString TtextQ = this->TParameter->text();
	std::string Ttext = TtextQ.toStdString();
	if(!Ttext.empty()){
		parameters.push_back("-T");
		char *temp = new char;
		strcpy(temp, Ttext.c_str());
		parameters.push_back(temp);
		//delete temp;
	}

	QString ItextQ = this->IParameter->text();
	std::string Itext = ItextQ.toStdString();
	if(!Itext.empty()){
		parameters.push_back("-I");
		char *temp = new char;
		strcpy(temp, Itext.c_str());
		parameters.push_back(temp);
		//delete temp;
	}

	QString qtextQ = this->qParameter->text();
	std::string qtext = qtextQ.toStdString();
	if(!qtext.empty()){
		parameters.push_back("-q");
		char *temp = new char;
		strcpy(temp, qtext.c_str());
		parameters.push_back(temp);
		//delete temp;
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

void EventQtSlotConnect::readCustomDataLabel(vtkFloatArray* labelDBL)
{
	MouseInteractorAdd::labelData.clear();
	for(int i = 0; i < labelDBL->GetSize(); i++)
	{
		MouseInteractorAdd::labelData.push_back(labelDBL->GetValue(i));
	}
}

void EventQtSlotConnect::readCustomDataTri(vtkFloatArray* triDBL)
{
	for(vtkIdType i = 0; i < triDBL->GetSize();)
	{
		//triDBL->GetValue(i);
		vtkSmartPointer<vtkPoints> pts =
			vtkSmartPointer<vtkPoints>::New();

		TagTriangle tri;
		tri.p1[0] = triDBL->GetValue(i);tri.p1[1] = triDBL->GetValue(i+1);tri.p1[2] = triDBL->GetValue(i+2);
		tri.id1 = triDBL->GetValue(i+3);
		tri.seq1 = triDBL->GetValue(i+4);
		tri.p2[0] = triDBL->GetValue(i+5);tri.p2[1] = triDBL->GetValue(i+6);tri.p2[2] = triDBL->GetValue(i+7);
		tri.id2 = triDBL->GetValue(i+8);
		tri.seq2 = triDBL->GetValue(i+9);
		tri.p3[0] = triDBL->GetValue(i+10);tri.p3[1] = triDBL->GetValue(i+11);tri.p3[2] = triDBL->GetValue(i+12);
		tri.id3 = triDBL->GetValue(i+13);
		tri.seq3 = triDBL->GetValue(i+14);


		for(int j = 0; j < 3; j++){
			double t1,t2,t3;
			t1 = triDBL->GetValue(i); t2 = triDBL->GetValue(i+1); t3 = triDBL->GetValue(i+2);
			pts->InsertNextPoint(t1,t2,t3);
			i += 5;
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
		actor->GetProperty()->SetColor(MouseInteractorAdd::triCol);
		actor->GetProperty()->EdgeVisibilityOn();
		actor->GetProperty()->SetEdgeColor(0.0,0.0,0.0);
		vtkSmartPointer<vtkProperty> backPro = 
			vtkSmartPointer<vtkProperty>::New();
		backPro->SetColor(MouseInteractorAdd::backCol);
		actor->SetBackfaceProperty(backPro);

		

		tri.centerPos[0] = actor->GetCenter()[0];
		tri.centerPos[1] = actor->GetCenter()[1];
		tri.centerPos[2] = actor->GetCenter()[2];
		tri.triActor = actor;
		MouseInteractorAdd::vectorTagTriangles.push_back(tri);
		this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);


		/*//draw normal
		double *n = new double[3];
		triangle->ComputeNormal(MouseInteractorAdd::vectorTagPoints[tri.id1].pos, 
			MouseInteractorAdd::vectorTagPoints[tri.id2].pos, 
			MouseInteractorAdd::vectorTagPoints[tri.id3].pos, n);
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

		MouseInteractorAdd::triNormalActors.push_back(actorNormal);
		this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actorNormal);*/
	}
}

void EventQtSlotConnect::readCustomDataEdge(vtkFloatArray* edgeDBL)
{
	for(int i = 0; i < edgeDBL->GetSize(); i += 5)
	{
		TagEdge edge;
		edge.ptId1 = edgeDBL->GetValue(i);
		edge.ptId2 = edgeDBL->GetValue(i+1);
		edge.seq = edgeDBL->GetValue(i+2);
		edge.numEdge = edgeDBL->GetValue(i+3);
		edge.constrain = edgeDBL->GetValue(i+4);

		MouseInteractorAdd::vectorTagEdges.push_back(edge);
	}
}

void EventQtSlotConnect::readCustomDataTag(vtkFloatArray* tagDBL, vtkStringArray* tagStr)
{
	for(int i = 0, j = 0; i < tagDBL->GetSize(); i += 5, j++)
	{
		TagInfo info;
		info.tagType = tagDBL->GetValue(i);
		info.tagIndex = tagDBL->GetValue(i+1);
		info.tagColor[0] = tagDBL->GetValue(i+2);
		info.tagColor[1] = tagDBL->GetValue(i+3);
		info.tagColor[2] = tagDBL->GetValue(i+4);
		info.qc = QColor(info.tagColor[0], info.tagColor[1], info.tagColor[2]);
		info.tagName = tagStr->GetValue(j).c_str();
		MouseInteractorAdd::vectorTagInfo.push_back(info);

		QPixmap pix(22,22);
		QString displayText = QString::number(info.tagIndex) + QString::fromStdString(" ") + (tagStr->GetValue(j));
		pix.fill(info.qc);
		this->comboBoxTagPoint->addItem(pix, displayText);
	}
}

void EventQtSlotConnect::readCustomDataPoints(vtkFloatArray* ptsDBL)
{
	for(int i = 0; i < ptsDBL->GetSize(); i += 7)
	{
		TagPoint tagPt;
		tagPt.pos[0] = ptsDBL->GetValue(i);
		tagPt.pos[1] = ptsDBL->GetValue(i+1);
		tagPt.pos[2] = ptsDBL->GetValue(i+2);
		tagPt.radius = ptsDBL->GetValue(i+3);
		tagPt.seq = ptsDBL->GetValue(i+4);
		tagPt.typeIndex = ptsDBL->GetValue(i+5);
		tagPt.comboBoxIndex = ptsDBL->GetValue(i+6);

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
		TagInfo ti = MouseInteractorAdd::vectorTagInfo[tagPt.comboBoxIndex];
		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetColor(ti.tagColor[0] / 255.0, ti.tagColor[1] / 255.0, ti.tagColor[2] / 255.0);

		tagPt.actor = actor;
		MouseInteractorAdd::vectorTagPoints.push_back(tagPt);
		this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
	}
}

void EventQtSlotConnect::readCustomData(vtkPolyData *polydata)
{
	vtkFloatArray* labelDBL = (vtkFloatArray*)polydata->GetFieldData()->GetArray("Label");
	readCustomDataLabel(labelDBL);	

	vtkFloatArray* tagDBL = (vtkFloatArray*)polydata->GetFieldData()->GetArray("TagInfo");
	vtkStringArray* tagStr = (vtkStringArray*)polydata->GetFieldData()->GetAbstractArray("TagName");
	std::cout<<" string size "<<tagStr->GetSize()<<std::endl;
	readCustomDataTag(tagDBL, tagStr);

	vtkFloatArray* ptsDBL = (vtkFloatArray*)polydata->GetFieldData()->GetArray("TagPoints");
	readCustomDataPoints(ptsDBL);
	std::cout<<"after tag point"<<std::endl;

	vtkFloatArray* triDBL = (vtkFloatArray*)polydata->GetFieldData()->GetArray("TagTriangles");
	readCustomDataTri(triDBL);
	std::cout<<"after tri point"<<std::endl;

	vtkFloatArray* edgeDBL = (vtkFloatArray*)polydata->GetFieldData()->GetArray("TagEdges");
	readCustomDataEdge(edgeDBL);
	std::cout<<"after tagEdge point"<<std::endl;
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

	std::cout<<"let see points "<<polydata->GetPoint(0)[0]<<std::endl;
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
		MouseInteractorAdd::isSkeleton = false;
	else
		MouseInteractorAdd::isSkeleton = true;

	MouseInteractorAdd::labelData.clear();
	MouseInteractorAdd::labelData.resize(polydata->GetPoints()->GetNumberOfPoints());
	for(int i = 0; i < MouseInteractorAdd::labelData.size(); i++)
		MouseInteractorAdd::labelData[i] = 0.0;

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
	vtkSmartPointer<MouseInteractorAdd> style = vtkSmartPointer<MouseInteractorAdd>::New();
//	style->qtObject = this;
	style->SetDefaultRenderer(renderer);
	style->labelTriNumber = this->TriangleNumber;
	style->labelPtNumber = this->PointNumber;

	//reset everything
	MouseInteractorAdd::vectorTagPoints.clear();
	MouseInteractorAdd::vectorClassifyPoints.clear();
	MouseInteractorAdd::vectorTagTriangles.clear();
	MouseInteractorAdd::vectorTagEdges.clear();
	MouseInteractorAdd::vectorTagInfo.clear();
	MouseInteractorAdd::triNormalActors.clear();
	MouseInteractorAdd::selectedTag = 0;
	this->comboBoxTagPoint->clear();
	this->checkBoxHideMesh->setChecked(false);
	this->checkBoxHideSkel->setChecked(false);

	this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style );
	if(this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer() != NULL)
		this->qvtkWidget->GetRenderWindow()->RemoveRenderer(this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
	this->qvtkWidget->update();	

	//get normals
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
	/*vtkSmartPointer<vtkActor> actor0 = static_cast<vtkActor *>(this->GetDefaultRenderer()->GetActors()->GetItemAsObject(0));
	vtkSmartPointer<vtkDataSet> vtkdata = actor0->GetMapper()->GetInputAsDataSet();	*/

#if VTK_MAJOR_VERSION <= 5
	normalGenerator->SetInput(polydata);
#else
	normalGenerator->SetInputData(polydata);
#endif
	normalGenerator->ComputePointNormalsOn();
	normalGenerator->ComputeCellNormalsOff();
	normalGenerator->Update();

	style->setNormalGenerator(normalGenerator);

	//see triangle
	vtkDoubleArray* triDBL = (vtkDoubleArray*)polydata->GetFieldData()->GetArray("TagTriangles");
	if(triDBL != NULL)
	{
		readCustomData(polydata);
	}
	this->PointNumber->setText(QString::number(MouseInteractorAdd::vectorTagPoints.size()));
	this->TriangleNumber->setText(QString::number(MouseInteractorAdd::vectorTagTriangles.size()));
}

void EventQtSlotConnect::saveVTKFile(QString fileName)
{
	vtkSmartPointer<vtkGenericDataObjectWriter> writer = 
		vtkSmartPointer<vtkGenericDataObjectWriter>::New();
	writer->SetFileName((fileName.toStdString().substr(0, fileName.toStdString().length() - 4)).append("Affix.vtk").c_str());	


	vtkSmartPointer<vtkPolyData> finalPolyData =
		vtkSmartPointer<vtkPolyData>::New();

	finalPolyData = polyObject;

	if(finalPolyData->GetFieldData()->GetArray("Label")){
		finalPolyData->GetFieldData()->RemoveArray("Label");
	}
	if(finalPolyData->GetFieldData()->GetArray("TagTriangles")){
		finalPolyData->GetFieldData()->RemoveArray("TagTriangles");
	}
	if(finalPolyData->GetFieldData()->GetArray("TagEdges")){
		finalPolyData->GetFieldData()->RemoveArray("TagEdges");
	}
	if(finalPolyData->GetFieldData()->GetArray("TagPoints")){
		finalPolyData->GetFieldData()->RemoveArray("TagPoints");
	}
	if(finalPolyData->GetFieldData()->GetArray("TagInfo")){
		finalPolyData->GetFieldData()->RemoveArray("TagInfo");
	}

	vtkSmartPointer<vtkFieldData> field =
		vtkSmartPointer<vtkFieldData>::New();

	vtkSmartPointer<vtkFloatArray> fltArray1 = 
		vtkSmartPointer<vtkFloatArray>::New();
	fltArray1->SetName("Label");
	for(int i = 0; i < MouseInteractorAdd::labelData.size(); i++)
		fltArray1->InsertNextValue(MouseInteractorAdd::labelData[i]);
	if(MouseInteractorAdd::labelData.size() !=0 )
		//finalPolyData->GetFieldData()->AddArray(fltArray1);
		field->AddArray(fltArray1);

	vtkSmartPointer<vtkFloatArray> fltArray2 = 
		vtkSmartPointer<vtkFloatArray>::New();
	fltArray2->SetName("TagTriangles");
	for(int i = 0; i < MouseInteractorAdd::vectorTagTriangles.size(); i++)
	{
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p1[0]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p1[1]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p1[2]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].id1);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].seq1);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p2[0]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p2[1]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p2[2]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].id2);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].seq2);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p3[0]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p3[1]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].p3[2]);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].id3);
		fltArray2->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].seq3);
	}
	if(MouseInteractorAdd::vectorTagTriangles.size() != 0)
		//finalPolyData->GetFieldData()->AddArray(fltArray2);
		field->AddArray(fltArray2);

	vtkSmartPointer<vtkFloatArray> fltArray3 = 
		vtkSmartPointer<vtkFloatArray>::New();
	fltArray3->SetName("TagEdges");
	for(int i = 0; i < MouseInteractorAdd::vectorTagEdges.size(); i++)
	{
		fltArray3->InsertNextValue(MouseInteractorAdd::vectorTagEdges[i].ptId1);
		fltArray3->InsertNextValue(MouseInteractorAdd::vectorTagEdges[i].ptId2);
		fltArray3->InsertNextValue(MouseInteractorAdd::vectorTagEdges[i].seq);
		fltArray3->InsertNextValue(MouseInteractorAdd::vectorTagEdges[i].numEdge);
		fltArray3->InsertNextValue(MouseInteractorAdd::vectorTagEdges[i].constrain);
	}
	if(MouseInteractorAdd::vectorTagEdges.size() != 0)
		//finalPolyData->GetFieldData()->AddArray(fltArray3);
		field->AddArray(fltArray3);

	vtkSmartPointer<vtkFloatArray> fltArray4 = 
		vtkSmartPointer<vtkFloatArray>::New();
	fltArray4->SetName("TagPoints");
	for(int i = 0; i < MouseInteractorAdd::vectorTagPoints.size(); i++)
	{
		fltArray4->InsertNextValue(MouseInteractorAdd::vectorTagPoints[i].pos[0]);
		fltArray4->InsertNextValue(MouseInteractorAdd::vectorTagPoints[i].pos[1]);
		fltArray4->InsertNextValue(MouseInteractorAdd::vectorTagPoints[i].pos[2]);
		fltArray4->InsertNextValue(MouseInteractorAdd::vectorTagPoints[i].radius);
		fltArray4->InsertNextValue(MouseInteractorAdd::vectorTagPoints[i].seq);
		fltArray4->InsertNextValue(MouseInteractorAdd::vectorTagPoints[i].typeIndex);
		fltArray4->InsertNextValue(MouseInteractorAdd::vectorTagPoints[i].comboBoxIndex);
	}
	if(MouseInteractorAdd::vectorTagPoints.size() != 0)
		//finalPolyData->GetFieldData()->AddArray(fltArray4);
		field->AddArray(fltArray4);

	vtkSmartPointer<vtkFloatArray> fltArray5 = 
		vtkSmartPointer<vtkFloatArray>::New();
	fltArray5->SetName("TagInfo");

	vtkSmartPointer<vtkStringArray> strArray1 = 
		vtkSmartPointer<vtkStringArray>::New();
	strArray1->SetName("TagName");

	for(int i = 0; i < MouseInteractorAdd::vectorTagInfo.size(); i++)
	{
		fltArray5->InsertNextValue(MouseInteractorAdd::vectorTagInfo[i].tagType);
		fltArray5->InsertNextValue(MouseInteractorAdd::vectorTagInfo[i].tagIndex);
		fltArray5->InsertNextValue(MouseInteractorAdd::vectorTagInfo[i].tagColor[0]);
		fltArray5->InsertNextValue(MouseInteractorAdd::vectorTagInfo[i].tagColor[1]);
		fltArray5->InsertNextValue(MouseInteractorAdd::vectorTagInfo[i].tagColor[2]);

		strArray1->InsertNextValue(MouseInteractorAdd::vectorTagInfo[i].tagName.c_str());
	}
	if(MouseInteractorAdd::vectorTagInfo.size() != 0)
	{
		field->AddArray(fltArray5);
		field->AddArray(strArray1);
	}


	vtkSmartPointer<vtkIntArray> intArray1 = 
		vtkSmartPointer<vtkIntArray>::New();
	intArray1->SetName("TriSeq");
	for(int i = 0; i < MouseInteractorAdd::vectorTagTriangles.size(); i++)
	{
		intArray1->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].seq1);
		intArray1->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].seq2);
		intArray1->InsertNextValue(MouseInteractorAdd::vectorTagTriangles[i].seq3);
	}
	//finalPolyData->GetFieldData()->AddArray(intArray1);
	field->AddArray(intArray1);

	finalPolyData->SetFieldData(field);
	writer->SetInput(finalPolyData);
	writer->Update();
	writer->Write();
}

void EventQtSlotConnect::saveParaViewFile(QString fileName)
{
	if(MouseInteractorAdd::vectorTagTriangles.size() > 0)
	{
		//////////////save another file for ParaView////////////////////
		vtkSmartPointer<vtkGenericDataObjectWriter> writerParaView = 
			vtkSmartPointer<vtkGenericDataObjectWriter>::New();

		writerParaView->SetFileName(fileName.toStdString().c_str());

		//Append the two meshes 
		vtkSmartPointer<vtkAppendPolyData> appendFilter =
			vtkSmartPointer<vtkAppendPolyData>::New();

		for(int i = 0; i < MouseInteractorAdd::vectorTagTriangles.size(); i++)
		{
			vtkSmartPointer<vtkActorCollection> actorCollection =
				vtkSmartPointer<vtkActorCollection>::New();
			MouseInteractorAdd::vectorTagTriangles[i].triActor->GetActors(actorCollection);		
			vtkPolyData* polyData = vtkPolyData::SafeDownCast(actorCollection->GetLastActor()->GetMapper()->GetInput());
			appendFilter->AddInput(polyData);
		}

		vtkSmartPointer<vtkCleanPolyData> cleanPoly = 
			vtkSmartPointer<vtkCleanPolyData>::New();

		cleanPoly->SetInput(appendFilter->GetOutput());
		cleanPoly->Update();

		//MouseInteractorAdd::labelData.clear();
		//MouseInteractorAdd::labelData.resize(0);
		std::vector<int> labelData;
		std::vector<double> radiusData;

		for(int i = 0; i < cleanPoly->GetOutput()->GetNumberOfPoints(); i++)
		{
			for(int j = 0; j < MouseInteractorAdd::vectorTagPoints.size(); j++)
			{
				if(MouseInteractorAdd::vectorTagPoints[j].pos[0] == cleanPoly->GetOutput()->GetPoint(i)[0] &&
					MouseInteractorAdd::vectorTagPoints[j].pos[1] == cleanPoly->GetOutput()->GetPoint(i)[1] &&
					MouseInteractorAdd::vectorTagPoints[j].pos[2] == cleanPoly->GetOutput()->GetPoint(i)[2])
				{
					labelData.push_back(MouseInteractorAdd::vectorTagPoints[j].typeIndex);
					radiusData.push_back(MouseInteractorAdd::vectorTagPoints[j].radius);
				}
			}
		}

		vtkSmartPointer<vtkFloatArray> fltArray6 = 
			vtkSmartPointer<vtkFloatArray>::New();
		fltArray6->SetName("Label");
		for(int i = 0; i < labelData.size(); i++)
			fltArray6->InsertNextValue(labelData[i]);

		vtkSmartPointer<vtkFloatArray> fltArray7 = 
			vtkSmartPointer<vtkFloatArray>::New();
		fltArray7->SetName("Radius");
		for(int i = 0; i < radiusData.size(); i++)
			fltArray7->InsertNextValue(radiusData[i]);

		cleanPoly->GetOutput()->GetPointData()->AddArray(fltArray6);
		cleanPoly->GetOutput()->GetPointData()->AddArray(fltArray7);
		writerParaView->SetInput(cleanPoly->GetOutput());
		writerParaView->SetFileTypeToBinary();//solve for matlab
		//writerParaView->SetFileTypeToASCII();
		writerParaView->Update();
		writerParaView->Write();
	}
}

void EventQtSlotConnect::saveCmrepFile(QString fileName)
{
	///////////////save cmrep file ////////////////////
	std::ofstream cmrepFile;

	cmrepFile.open((fileName.toStdString().substr(0, fileName.toStdString().length() - 4)).append(".cmrep").c_str());

	cmrepFile<<"Grid.Type = ";
	if(this->GridTypeComboBox->currentIndex() == 0){
		cmrepFile<<"LoopSubdivision"<<endl;
	}

	cmrepFile<<"Grid.Model.SolverType = ";
	if(this->SolverTypeComboBox->currentIndex() == 0)
		cmrepFile<<"BruteForce"<<endl;
	else if(this->SolverTypeComboBox->currentIndex() == 1)
		cmrepFile<<"PDE"<<endl;

	if(this->GridTypeComboBox->currentIndex() == 0){
		cmrepFile<<"Grid.Model.Atom.SubdivisionLevel = ";
		switch(this->SubLevelComboBox->currentIndex()){
		case 0: 
			cmrepFile<<"0"<<endl;
			break;
		case 1: 
			cmrepFile<<"1"<<endl;
			break;
		case 2:
			cmrepFile<<"2"<<endl;
			break;
		case 3:
			cmrepFile<<"3"<<endl;
			break;
		case 4:
			cmrepFile<<"4"<<endl;
			break;
		}
	}

	cmrepFile<<"Grid.Model.Coefficient.FileName = ";
	std::string name = fileName.toStdString();
	int lastSlash;

#ifdef _WIN64
	lastSlash = name.find_last_of("/");
#elif _WIN32
	lastSlash = name.find_last_of("/");
#elif _APPLE_
	lastSlash = name.find_last_of("\\");
#elif _linux
	lastSlash = name.find_last_of("\\");
#endif
	
	cmrepFile<<name.substr(lastSlash+1, name.size())<<endl;

	cmrepFile<<"Grid.Model.Coefficient.FileType = VTK"<<endl;

	if(this->SolverTypeComboBox->currentIndex() == 1){
		cmrepFile<<"Grid.Model.Coefficient.ConstantRho = ";
		cmrepFile<<this->RhoLineEdit->text().toStdString()<<endl;
	}

	if(this->ConsRadiusCheckBox->isChecked()){
		cmrepFile<<"Grid.Model.Coefficient.ConstantRadius = ";
		cmrepFile<<this->RadiusLineEdit->text().toStdString()<<endl;
	}

	cmrepFile<<"Grid.Model.nLabels = ";
	std::vector<bool>trackNumLabel;
	trackNumLabel.resize(10);
	for(int i = 0; i < MouseInteractorAdd::vectorTagInfo.size(); i ++)
	{
		trackNumLabel[MouseInteractorAdd::vectorTagInfo[i].tagIndex] = true;
	}

	int numCount = 0;
	for(int i = 0; i < trackNumLabel.size(); i++)
		if(trackNumLabel[i])
			numCount ++;
	cmrepFile<<numCount;

	cmrepFile.close();
}

void EventQtSlotConnect::Decimate()
{
	if(this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer() != NULL)
	{
		MouseInteractorAdd::decimateMode = true;

		//clear all triangle
		for(int i = 0; i < MouseInteractorAdd::vectorTagTriangles.size(); i++)
		{
			this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(MouseInteractorAdd::vectorTagTriangles[i].triActor);
		}
		MouseInteractorAdd::vectorTagTriangles.clear();
		for(int i = 0; i < MouseInteractorAdd::vectorTagPoints.size(); i++)
			this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(MouseInteractorAdd::vectorTagPoints[i].actor);
		MouseInteractorAdd::vectorTagPoints.clear();
		MouseInteractorAdd::vectorTagEdges.clear();

		//find the first actor
		vtkSmartPointer<vtkActorCollection> actors = this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActors();
		//this->GetDefaultRenderer()->GetActors();	
		vtkSmartPointer<vtkActor> actor0 =  static_cast<vtkActor *>(actors->GetItemAsObject(0));	
		vtkSmartPointer<vtkDataSet> vtkdata = actor0->GetMapper()->GetInputAsDataSet();

		vtkSmartPointer<vtkTriangleFilter> triangleFilter =
			vtkSmartPointer<vtkTriangleFilter>::New();
		triangleFilter->SetInputConnection(vtkdata->GetProducerPort());
		triangleFilter->Update();


		vtkSmartPointer<vtkDecimatePro> decimate =
			vtkSmartPointer<vtkDecimatePro>::New();
		decimate->SetInputConnection(triangleFilter->GetOutputPort());
		cout<<"what is targetReduction "<<targetReduction<<endl;
		decimate->SetTargetReduction(targetReduction);
		decimate->SetFeatureAngle(featureAngle);
		decimate->Update();

		vtkSmartPointer<vtkPolyData> decimated =
			vtkSmartPointer<vtkPolyData>::New();
		decimated->ShallowCopy(decimate->GetOutput());

		/*vtkSmartPointer<vtkPolyDataMapper> mapper = 
		vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInput(decimated);*/

		/*vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);

		this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);*/
		vtkIdType npts, *ptsTri;
		decimated->GetPolys()->InitTraversal();
		while(decimated->GetPolys()->GetNextCell(npts,ptsTri)) 
		{		
			vtkSmartPointer<vtkPoints> pts =
				vtkSmartPointer<vtkPoints>::New();
			for(int i = 0; i < 3; i++){
				pts->InsertNextPoint(decimated->GetPoint(ptsTri[i]));
			}

			int seq1, seq2, seq3;

			for(int i = 0;i < vtkdata->GetNumberOfPoints(); i++)
			{
				if(vtkdata->GetPoint(i)[0] == decimated->GetPoint(ptsTri[0])[0]
				&& vtkdata->GetPoint(i)[1] == decimated->GetPoint(ptsTri[0])[1]
				&& vtkdata->GetPoint(i)[2] == decimated->GetPoint(ptsTri[0])[2])
				{
					seq1 = i;
				}

				if(vtkdata->GetPoint(i)[0] == decimated->GetPoint(ptsTri[1])[0]
				&& vtkdata->GetPoint(i)[1] == decimated->GetPoint(ptsTri[1])[1]
				&& vtkdata->GetPoint(i)[2] == decimated->GetPoint(ptsTri[1])[2])
				{
					seq2 = i;
				}

				if(vtkdata->GetPoint(i)[0] == decimated->GetPoint(ptsTri[2])[0]
				&& vtkdata->GetPoint(i)[1] == decimated->GetPoint(ptsTri[2])[1]
				&& vtkdata->GetPoint(i)[2] == decimated->GetPoint(ptsTri[2])[2])
				{
					seq3 = i;
				}
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
			//mapper->SetInput(appendFilter->GetOutput());
	#else
			mapper->SetInputData(trianglePolyData);
	#endif
			vtkSmartPointer<vtkActor> actor =
				vtkSmartPointer<vtkActor>::New();
			actor->SetMapper(mapper);
			actor->GetProperty()->SetEdgeVisibility(true);
			actor->GetProperty()->SetEdgeColor(0.0,0.0,0.0);
			actor->GetProperty()->SetColor(0.2, 0.7, 0.2);
			vtkSmartPointer<vtkProperty> backPro = 
				vtkSmartPointer<vtkProperty>::New();
			backPro->SetColor(0.4, 0.4, 0.4);
			actor->SetBackfaceProperty(backPro);

			this->qvtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
			//this->GetDefaultRenderer()->AddActor(actor);

			TagTriangle tri;
			tri.triActor = actor;
			tri.p1[0] = decimated->GetPoint(ptsTri[0])[0]; tri.p1[1] = decimated->GetPoint(ptsTri[0])[1]; tri.p1[2] = decimated->GetPoint(ptsTri[0])[2];
			tri.p2[0] = decimated->GetPoint(ptsTri[1])[0]; tri.p2[1] = decimated->GetPoint(ptsTri[1])[1]; tri.p2[2] = decimated->GetPoint(ptsTri[1])[2];
			tri.p3[0] = decimated->GetPoint(ptsTri[2])[0]; tri.p3[1] = decimated->GetPoint(ptsTri[2])[1]; tri.p3[2] = decimated->GetPoint(ptsTri[2])[2];
			tri.id1 = -1; tri.id2 = -1; tri.id3 = -1;
			tri.centerPos[0] = trianglePolyData->GetCenter()[0];
			tri.centerPos[1] = trianglePolyData->GetCenter()[1];
			tri.centerPos[2] = trianglePolyData->GetCenter()[2];
			tri.seq1 = seq1;//ptsTri[0];
			tri.seq2 = seq2;//ptsTri[1];
			tri.seq3 = seq3;//ptsTri[2];
			MouseInteractorAdd::vectorTagTriangles.push_back(tri);
		}
	}
}