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

#include <QtGui>

#include "constants.h"
//#include "EventQtSlotConnect.h"

// Handle mouse events
class MouseInteractorAdd : public vtkInteractorStyleTrackballCamera
{
public:
	static MouseInteractorAdd* New();
	vtkTypeMacro(MouseInteractorAdd, vtkInteractorStyleTrackballCamera);	

	MouseInteractorAdd();
	//EventQtSlotConnect *qtObject;
	
	std::vector<int>deletePointIds;
	std::vector<int>triPtIds;
	vtkActor *selectedTriangle;
	vtkActor *prePolyLineActor;

	static std::vector<TagInfo> vectorTagInfo;
	static std::vector<TagTriangle> vectorTagTriangles;
	static std::vector<TagPoint> vectorTagPoints;
	static std::vector<std::vector<TagPoint>> vectorClassifyPoints;
	static std::vector<TagEdge> vectorTagEdges;
	//store all the label info, 0 represent no tag on this point
	static std::vector<double> labelData;
	static bool isSkeleton;
	static int selectedTag;
	

	double Distance(double p1[3], double p2[3]);
	void SetSelectedTriColor();
	int ConstrainEdge(int type1, int type2);
	int PairNumber(int a, int b);
	void DrawTriangle();
	void DeleteTriangle(vtkActor* pickedActor);
	vtkActor* PickAcotrFromMesh(double pos[3]);
	void copyEdgeBtoA(int a, int b);
	int deleteEdgeHelper(int id1, int id2, int seq);
	int deleteEdgeHelper2(int id, int seq);
	void deleteEdge(int seq);

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

							//QComboBox* cbTagPoint;// =  qtObject->getTagComboBox();
							TagInfo ti = vectorTagInfo[selectedTag];
							vtkSmartPointer<vtkActor> actor =
								vtkSmartPointer<vtkActor>::New();
							actor->SetMapper(mapper);
							actor->GetProperty()->SetColor(ti.tagColor[0] / 255.0, ti.tagColor[1] / 255.0, ti.tagColor[2] / 255.0);
						
							//store actor in vectorTagPoints
							TagPoint actorT;
							actorT.actor = actor;
							actorT.typeIndex = selectedTag;
							//actorT.typeName = cbTagPoint->currentText().toStdString(); 
							actorT.radius = pointRadius;
							actorT.seq = pointSeq;
							actorT.pos[0] = finalPos[0]; actorT.pos[1] = finalPos[1]; actorT.pos[2] = finalPos[2];
							vectorTagPoints.push_back(actorT);

							//calculate the biggest number of edges possibility
							vectorTagEdges.resize(PairNumber(vectorTagPoints.size(), vectorTagPoints.size()));
							
							labelData[pointSeq] = selectedTag + 1;//cbTagPoint->currentIndex() + 1;
						
							if(vectorClassifyPoints.size() <= selectedTag/*cbTagPoint->currentIndex()*/){
								vectorClassifyPoints.resize(selectedTag/*cbTagPoint->currentIndex()*/ + 1);
							}
							vectorClassifyPoints[selectedTag/*cbTagPoint->currentIndex()*/].push_back(actorT);

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
			}//end of key press
		}
		vtkInteractorStyleTrackballCamera::OnLeftButtonDown();				
	}

private:
	double triCol[3];

};
#endif