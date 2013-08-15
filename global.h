#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <QColor>
#include <vtkActor.h>
#include <vector>

struct TagInfo
{
	std::string tagName;
	int tagType; // 1 = Branch point  2 = Free Edge point 3 = Interior point  4 = others
	int tagColor[3];//for color
	QColor qc;//for color	
	int tagIndex;
};


struct TagTriangle
{
	vtkActor *triActor;
	double p1[3], p2[3], p3[3];
	double centerPos[3];
	int id1, id2, id3;//for point index in tagPoint
	int seq1, seq2, seq3;//for point index in total point
	int index;
	//double triColor[3];
};


struct TagPoint
{
	vtkActor* actor;
	std::string typeName;
	int typeIndex;
	int comboBoxIndex;
	double radius;//radius of that points
	int seq;//the sequence in total points
	double pos[3];
	int ptIndex;
};	


struct TagEdge
{
	int ptId1;
	int ptId2;
	int constrain;
	int numEdge;
	int seq;//use of deletion
};

struct TagAction
{
	int action;
	double pos[3];
	int triIndex;//for triangle label
	TagPoint pointInfo;
	TagTriangle triangleInfo;
	int ptIndex;
	int ptOldSeq;
};

class Global
{
public:
	static std::vector<TagInfo> vectorTagInfo;
	static std::vector<TagTriangle> vectorTagTriangles;
	static std::vector<TagPoint> vectorTagPoints;	
	static std::vector<TagEdge> vectorTagEdges;

	//store all the label info, 0 represent no tag on this point
	static std::vector<double> labelData;
	static std::vector<vtkActor*> triNormalActors;
	static bool isSkeleton;
	static int selectedTag;
	static double targetReduction;
	static double featureAngle;
	static double tagRadius;

	static double triCol[3];
	static double backCol[3];

	static bool decimateMode;
};

#endif
