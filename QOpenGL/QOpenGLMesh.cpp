#include "QOpenGLMesh.h"

void QOpenGLMesh::initializeVertices(GLuint vertexLength, GLuint indexLength)
{
	this->texcoords=newGLfloat(vertexLength*2);
	this->positions=newGLfloat(vertexLength*3);
	this->normals=newGLfloat(vertexLength*3);
	this->colors=newGLfloat(vertexLength*3);
	this->indices=newGLushort(indexLength);
	this->vertexLength=vertexLength;
	this->indexLength=indexLength;
	this->texcoordIndex=0;
	this->positionIndex=0;
	this->normalIndex=0;
	this->vertexIndex=0;
	this->colorIndex=0;
	this->index=0;
}
void QOpenGLMesh::addTexcoord(GLfloat x, GLfloat y)
{
	this->texcoords[texcoordIndex++]=x;
	this->texcoords[texcoordIndex++]=y;
}
void QOpenGLMesh::addPosition(GLfloat x, GLfloat y, GLfloat z)
{
	this->positions[positionIndex++]=x;
	this->positions[positionIndex++]=y;
	this->positions[positionIndex++]=z;
}
void QOpenGLMesh::addNormal(GLfloat x, GLfloat y, GLfloat z)
{
	this->normals[normalIndex++]=x;
	this->normals[normalIndex++]=y;
	this->normals[normalIndex++]=z;
}
void QOpenGLMesh::addColor(QColor color)
{
	this->colors[colorIndex++]=color.redF();
	this->colors[colorIndex++]=color.greenF();
	this->colors[colorIndex++]=color.blueF();
}
void QOpenGLMesh::addIndex(GLushort i)
{
	this->indices[index++]=i;
}
void QOpenGLMesh::addIndices(GLushort i, GLushort j, GLushort k)
{
	this->indices[index++]=i;
	this->indices[index++]=j;
	this->indices[index++]=k;
}
void QOpenGLMesh::addTexcoord(GLfloat* texcoord)
{
	this->texcoords[texcoordIndex++]=texcoord[0];
	this->texcoords[texcoordIndex++]=texcoord[1];
}
void QOpenGLMesh::addPosition(GLfloat* position)
{
	this->positions[positionIndex++]=position[0];
	this->positions[positionIndex++]=position[1];
	this->positions[positionIndex++]=position[2];
}
void QOpenGLMesh::addNormal(GLfloat* normal)
{
	this->normals[normalIndex++]=normal[0];
	this->normals[normalIndex++]=normal[1];
	this->normals[normalIndex++]=normal[2];
}
void QOpenGLMesh::addVertex(GLvertex vertex)
{
	this->addTexcoord(vertex.texcoord);
	this->addPosition(vertex.position);
	this->addNormal(vertex.normal);
	this->vertexIndex++;
}
void QOpenGLMesh::addQuad(GLvertex v0, GLvertex v1, GLvertex v2, GLvertex v3)
{
	this->addIndices(vertexIndex+0, vertexIndex+1, vertexIndex+2);
	this->addIndices(vertexIndex+2, vertexIndex+3, vertexIndex+0);
	this->addVertex(v0); this->addVertex(v1);
	this->addVertex(v2); this->addVertex(v3);
}
void QOpenGLMesh::createBuffers()
{
	this->texcoordBuffer=newQOpenGLBuffer(QOpenGLBuffer::VertexBuffer, texcoords, sizeof(GLfloat), vertexLength*2);
	this->positionBuffer=newQOpenGLBuffer(QOpenGLBuffer::VertexBuffer, positions, sizeof(GLfloat), vertexLength*3);
	this->normalBuffer=newQOpenGLBuffer(QOpenGLBuffer::VertexBuffer, normals, sizeof(GLfloat), vertexLength*3);
	this->colorBuffer=newQOpenGLBuffer(QOpenGLBuffer::VertexBuffer, colors, sizeof(GLfloat), vertexLength*3);
	this->indexBuffer=newQOpenGLBuffer(QOpenGLBuffer::IndexBuffer, indices, sizeof(GLushort), indexLength);
}
GLfloat* QOpenGLMesh::newGLfloat(GLuint length)
{
	return (GLfloat*)malloc(sizeof(GLfloat)*length);
}
GLushort* QOpenGLMesh::newGLushort(GLuint length)
{
	return (GLushort*)malloc(sizeof(GLushort)*length);
}
QOpenGLBuffer* QOpenGLMesh::newQOpenGLBuffer(QOpenGLBuffer::Type type, GLvoid* buffer, GLuint size, GLuint length)
{
	QOpenGLBuffer* openGLBuffer=new QOpenGLBuffer(type); 
	openGLBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw); 
	openGLBuffer->create(); openGLBuffer->bind();
	openGLBuffer->allocate(buffer, size*length);
	openGLBuffer->release(); return openGLBuffer;
}
QOpenGLMesh::~QOpenGLMesh()
{
	delete texcoordBuffer;
	delete positionBuffer;
	delete normalBuffer;
	delete indexBuffer;
	delete colorBuffer;
	free(texcoords);
	free(positions);
	free(normals);
	free(indices);
	free(colors);
}
