#include "QSketch.h"
#include "QCurve.h"
#include <QPainter>

class QShape : public QSketch
{
	public:
	void clear();
	QShape():QSketch(){}
	void switchLineCurveMode();
	void operator=(QPoint point);
	void operator<<(QPoint point);
	void drawPoints(QPainter& painter);
	bool isEditable=false, isAutoAligned=true;
	void drawLabelPoints(QPainter& painter);

	private:
	QPoint last();
	int curveIndex=-1;
	void updateLast();
	bool isCurveStarted();
	void start(QPoint point);
	void end(QPoint point);
	QVector<QCurve> curves;
	QPoint pointAt(int index);
	void getIndex(QPoint point);
	vec2 getPoint(QPoint point);
	QPoint toQPoint(vec2 point);
	void addLineTo(QPoint point);
	void addCurveTo(QPoint point);
	void startCurveAt(QPoint point);
	QPoint NONE=QPoint(-100, -100);
	bool isStarted=false; int index=-1;
	bool equals(QPoint point1, QPoint point2);
	qreal error=20; bool is(int type); int type=LINE;
	QPoint beginPoint=NONE, endPoint=NONE, curveStartPoint=NONE;
};
