#include <iostream>
#include <vector>

#include <QObject>

class VoronoiSkeletonTool: public QObject{
	Q_OBJECT
public:
	VoronoiSkeletonTool(){};
	~VoronoiSkeletonTool(){};
	//int execute(int argc, char *argv[]);
	int execute(int argc, std::vector <char*> argv);
 

signals:
	void progressChanged();
};
