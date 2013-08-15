#include "global.h"

std::vector<TagInfo> Global::vectorTagInfo;
std::vector<TagTriangle> Global::vectorTagTriangles;
std::vector<TagPoint> Global::vectorTagPoints;
std::vector<TagEdge> Global::vectorTagEdges;
//store all the label info, 0 represent no tag on this point
std::vector<double> Global::labelData;
std::vector<vtkActor*> Global::triNormalActors;
bool Global::isSkeleton;
int Global::selectedTag;
double Global::triCol[3];
double Global::backCol[3];
double Global::targetReduction;
double Global::featureAngle;
bool Global::decimateMode;
double Global::tagRadius;