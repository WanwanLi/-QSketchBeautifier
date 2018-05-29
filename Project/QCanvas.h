#include <QPen>
#include <QColor>
#include <QImage>
#include <QWidget>
#include "QShape.h"
#include <QPaintEvent>
#include <QBasicTimer>


class QCanvas : public QWidget
{
	Q_OBJECT
	public:
	QShape shape;
	QCanvas(QWidget* widget=0);
	bool loadImage(QString fileName);
	bool saveImage(QString fileName, const char* fileFormat);

	public slots:
	void clear();
	void getCircles();
	bool isModified();
	void setEditable();
	void resizeCanvas();
	void beautifySketch();
	void setAutoAligned();
	void switchLineCurveMode();

	private:
	QColor bgColor;
	QImage image;
	QWidget* widget;
	QPen pen, marker;
	QPen progressBar;
	void resizeImage();
	bool isMousePressed;
	bool isImageModified;
	QBasicTimer basicTimer;
	void critical(QString operation);
	void resizeWindow(int margin);
	void paintEvent(QPaintEvent* event);
	void timerEvent(QTimerEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
};
