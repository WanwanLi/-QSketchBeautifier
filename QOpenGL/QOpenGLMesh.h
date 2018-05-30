#include "QOpenGLVertex.h"
#include <QOpenGLBuffer>
#include <QColor>

class QOpenGLMesh
{
	public:
	void initializeVertices(GLuint vertexLength, GLuint indexLength);
	void addQuad(GLvertex v0, GLvertex v1, GLvertex v2, GLvertex v3);
	void addIndices(GLushort i, GLushort j, GLushort k);
	void addPosition(GLfloat x, GLfloat y, GLfloat z);
	void addNormal(GLfloat x, GLfloat y, GLfloat z);
	void addTexcoord(GLfloat x, GLfloat y);
	void addTexcoord(GLfloat* texcoord);
	void addPosition(GLfloat* position);
	void addNormal(GLfloat* normal);
	void addVertex(GLvertex vertex);
	QOpenGLBuffer* texcoordBuffer;
	QOpenGLBuffer* positionBuffer;
	QOpenGLBuffer* normalBuffer;
	QOpenGLBuffer* colorBuffer;
	QOpenGLBuffer* indexBuffer;
	void addColor(QColor color);
	void addIndex(GLushort i);
	GLuint vertexLength; 
	GLuint indexLength;
	GLuint vertexIndex;
	void createBuffers();
	~QOpenGLMesh();

	private:
	GLuint index;
	GLfloat* colors;
	GLfloat* normals;
	GLfloat* positions;
	GLfloat* texcoords;
	GLushort* indices;
	GLuint texcoordIndex;
	GLuint positionIndex;
	GLuint normalIndex;
	GLuint colorIndex;
	GLfloat* newGLfloat(GLuint length);
	GLushort* newGLushort(GLuint length);
	QOpenGLBuffer* newQOpenGLBuffer(QOpenGLBuffer::Type type, GLvoid* buffer, GLuint size, GLuint length);
};
