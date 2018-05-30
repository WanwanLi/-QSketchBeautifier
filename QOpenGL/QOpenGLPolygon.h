#include <QVector>
#include <QVector3D>
#include "QOpenGLObject.h"

class QOpenGLPolygon : public QOpenGLObject
{
	private:
	QColor color;
	QVector3D normal;
	qreal thickness=0.02;
	void setQuad(QVector3D* quad);
	void createVertices(), addVertex(int p, int n, int t);
	QVector3D p00, p01, p10, p11, getPosition(qreal u, qreal v);
	void translateQuad(QVector3D* quad, QVector3D translation);
	void loadPolygon(QVector3D* quad, QVector<QVector<qreal>> coords);
	void addPolygonSide(QVector3D* quad, QVector<qreal> coords, QVector3D normal);
	QVector<QVector3D> positions, normals; QVector<QColor> colors; QVector<QVector2D> texcoords;

	public:
	QOpenGLPolygon(QVector<QVector3D*> quads, QVector<QColor> colors, QVector<QVector<QVector<qreal>>> coords);
};
