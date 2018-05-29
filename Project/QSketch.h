#include "QDefine.h"
#include <QThread>
#include <QPainter>
#include <QPainterPath>
#include <QTextStream>

class QAnalyzer;
class QOptimizer;
class QSketch : public QObject
{
	Q_OBJECT

	public:
	QSketch(QSize size);
	bool isUpdated=false;
	bool isOnUpdating();
	QPainterPath painterPath;
	QAnalyzer* analyzer=NULL;
	QSketch(); bool beautify();
	enum {MOVE, LINE, CUBIC};
	bool load(QString fileName);
	bool save(QString fileName);
	void operator=(QPoint point);
	void operator+=(QPoint point);
	QString fileName="QSketch.sky";
	QSize size; void resize(QSize size);
	QVector<vec2> point2D; veci path;
	void drawPath(QPainter& painter);
	void save(QTextStream& textStream);
	void saveAsSVGFile(QString fileName);
	void drawRegularity(QPainter& painter);
	void drawProgressBar(QPainter& painter);
	void moveTo(QPoint point), lineTo(QPoint point);
	void update(), clear(), removeLast(), getCircles();
	void moveTo(qreal x, qreal y), lineTo(qreal x, qreal y);
	void cubicTo(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3);

    private slots:
    void finished();
    void setValue(int value);

	private:
	int iterations=0; void copySamePoints();
	QThread*  beautifier; QOptimizer* optimizer;
	QOptimizer* newQOptimizer(QThread* thread, int iterations);
};
