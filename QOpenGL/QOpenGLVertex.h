#include <QVector2D>
#include <QVector3D>
#include <QOpenGLFunctions>

class QOpenGLVertex
{
	public:
	QOpenGLVertex();
	QOpenGLVertex(QVector3D position, QVector3D normal, QVector2D texcoord);
	void setPosition(GLfloat x, GLfloat y, GLfloat z);
	void setNormal(GLfloat x, GLfloat y, GLfloat z);
	void setTexcoord(GLfloat x, GLfloat y);
	GLfloat* texcoord;
	GLfloat* position;
	GLfloat* normal;

	private:
	GLfloat* newGLvertex2f(GLfloat f0, GLfloat f1);
	GLfloat* newGLvertex3f(GLfloat f0, GLfloat f1, GLfloat f2);
};
typedef QOpenGLVertex GLvertex;