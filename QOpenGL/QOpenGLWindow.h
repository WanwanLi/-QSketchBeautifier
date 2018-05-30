#include <QBasicTimer>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include "QOpenGLPolygon.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class QOpenGLWindow : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
	public:
	QOpenGLWindow
	(
		const QVector<QColor>& colors,
		const QVector<QVector3D*>& quads, 
		const QVector<QVector<QVector<qreal>>>& coords
	):
	QOpenGLWidget(), colors(colors), quads(quads), coords(coords){}
	void initializeGL(), paintGL(), teardownGL();
	void resizeGL(int width, int height);
	~QOpenGLWindow();

	private:
	bool isMousePressed;
	QBasicTimer basicTimer;
	QVector<QColor> colors;
	void updateViewDistance();
	void updateViewDirection();
	QOpenGLTexture* glTexture;
	QOpenGLPolygon* glPolygon;
	QVector<QVector3D*> quads;
	QPoint mouseMove, mousePos;
	qreal mouseScroll, mouseDelta;
	qreal zNear=1, zFar=100, fov=45;
	void initializeOpenGLParameters();
	QOpenGLShaderProgram*glProgram;
	void timerEvent(QTimerEvent* event);
	void wheelEvent(QWheelEvent* event);
	QVector3D eye, center, right, up, forward;
	QVector<QVector<QVector<qreal>>> coords;
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	qreal clamp(qreal value, qreal min, qreal max);
	QOpenGLTexture* newQOpenGLTexture(QString fileName);
	QVector3D rotate(QVector3D vector, QVector3D axis, qreal angle);
	QMatrix4x4 rotateMatrix, glProjectionMatrix, glModelMatrix, glViewMatrix;
	qreal rotateAngle, angularSpeed, rotateSpeed, translateSpeed, viewDistance;
	QOpenGLShaderProgram* newQOpenGLShaderProgram(QString vShader, QString fShader);
	void glBindAttributeBuffer(const char* name, QOpenGLBuffer* glBuffer, GLuint tupleSize);
};
