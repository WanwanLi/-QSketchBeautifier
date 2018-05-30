#include "QOpenGLMesh.h"

class QOpenGLObject : public QOpenGLMesh
{
	public:
	QOpenGLObject(QString fileName);
	QOpenGLObject():QOpenGLMesh(){}

	private:
	void loadCube();
	void loadTrianleArray();
	void addTrianleVertex(int i);
	boolean loadObject(QString fileName);
	QStringList positionList, texcoordList, normalList;
	QStringList  positionIndexList, texcoordIndexList, normalIndexList;
	void loadTrianleIndex(QString index1, QString index2, QString index3);
};