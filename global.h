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
	int tagIndex;//the index of the tag(1-10)
};


struct TagTriangle
{
	vtkActor *triActor;
	double p1[3], p2[3], p3[3];
	double centerPos[3];
	int id1, id2, id3;//for point index in tagPoint
	int seq1, seq2, seq3;//for point index in all vertices  on skeleton
	int index;//the triangle label index
};


struct TagPoint
{
	vtkActor* actor;
	std::string typeName;
	int typeIndex;//tag index
	int comboBoxIndex;//index in combobox
	double radius;//radius of that points
	int seq;//the sequence in all vertices  on skeleton
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
	//Store Tag Information
	static std::vector<TagInfo> vectorTagInfo;
	//Store Triangle information
	static std::vector<TagTriangle> vectorTagTriangles;
	//Store Points information
	static std::vector<TagPoint> vectorTagPoints;	
	//Store Edge information
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
