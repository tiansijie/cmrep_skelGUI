#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_
#include <QColor>
#include <vtkActor.h>

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
};

#endif
