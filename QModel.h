#include "QDefine.h"
#include <QTransform>

class QModel
{
	public:
	QModel(){}
	void flatpack();
	qreal outputSize=800;
	void save(QString fileName);
	void saveAsSVGFile(QString modelName);
	QVector<QVector<vec>> coords; QVector<vec3*> quads;
	QModel(veci path, vec point4D, veci holes, QVector<vec3*> quads);

	private:
	void update();
	void getCoordinates();
	QVector<vec4> boxes;
	qreal curveLength=0.05;
	void scaleCoordinates();
	void rotateCoordinates();
	qreal aspectRatio(int quadIndex);
	vec3 pointAt(vec3* ctrlPoints, qreal t);
	QVector<QVector<QTransform>> transforms;
	QVector<vec2> getPlacement(qreal& maxWidth);
	void addPoint(vec& coordinates, vec3 point, vec3* quad);
	void addCurve(vec& coordinates, vec3* ctrlPoints,  vec3* quad);
};
