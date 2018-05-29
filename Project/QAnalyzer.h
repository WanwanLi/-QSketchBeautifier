#include "QDefine.h"
#include <QPainter>

class QSketch;
class QAnalyzer
{
	public:
	enum
	{
		LOOP,
		EQUAL,
		PARALLEL, 
		PERPENDICULAR
	};
	void getRegularity();
	void getCircles();
	void getChords();
	void run(), clear();
	QSketch* sketch;
	veci path, regularity;
	void iterate(int& index);
	qreal distanceError=20;
	qreal parallelError=0.01;
	static int count(int value);
	vec2 tangentAt(int index);
	QVector<vec4> endPoints;
	int toValue(QString string);
	void getCircle(vec2 chord);
	QString toString(int value);
	bool isLoop(int startIndex);
	void load(QSketch* sketch);
	void save(QString fileName);
	qreal circleFittingError=0.5;
	qreal equalLengthError=0.05;
	bool isParallel(vec2 x, vec2 y);
	qreal perpendicularError=0.02;
	int prev(int index), next(int index);
	void operator<<(QStringList& list);
	bool equals(int index1, int index2);
	bool isClose(int index1, int index2);
	void toCircle(QVector<vec2> args);
	bool isPerpendicular(vec2 x, vec2 y);
	vec2 toChordIndices(int chordIndex);
	QVector<vec2> getPoints(vec2 chord);
	void drawChords(QPainter& painter);
	void drawRegularity(QPainter& painter);
	void alignLeftTangent(int index, vec2 center);
	void alignRightTangent(int index, vec2 center);
	void getRegularity(int startIndex, int endIndex);
	QVector<vec2> point2D, curves, loops, chords, circles;
	QVector<vec4> getEndPointsOfChords(QVector<vec2>points);
	void updateChords(QVector<vec2>& points, QVector<vec4> endPoints);
};
