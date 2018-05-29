#include "QDefine.h"

class QCurve
{
	public:
	QCurve(){}
	QVector<vec2> point2D;
	void operator<<(QPoint point);
	QCurve(int startIndex, QPoint point);
	void update(veci& path, QVector<vec2>& point2D);

	private:
	int startIndex;
	void updateControlPoints();
	QVector<vec2*> controlPoints;
	vec2* getControlPoints(int index);
	void setPath(veci& path, int index, int value);
	void setPoint2D(QVector<vec2>& point2D, int index, vec2 point);
};
