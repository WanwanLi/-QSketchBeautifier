#include <QFile>
#include <QTextStream>
#include "QOpenGLObject.h"

QOpenGLObject::QOpenGLObject(QString fileName) : QOpenGLMesh()
{
	if(this->loadObject(fileName));
	else this->loadCube();
	this->createBuffers();
}
boolean QOpenGLObject::loadObject(QString fileName)
{
	QFile file(fileName); QStringList line;
	if(!file.open(QIODevice::ReadOnly))return false;
	QTextStream textStream(&file);
	while(!textStream.atEnd())
	{
		line=textStream.readLine().split(" ");
		if(line[0]=="vt")texcoordList<<line[1]<<line[2];
		else if(line[0]=="v")positionList<<line[1]<<line[2]<<line[3];
		else if(line[0]=="vn")normalList<<line[1]<<line[2]<<line[3];
		else if(line[0]=="f")loadTrianleIndex(line[1], line[2], line[3]);
	}
	this->loadTrianleArray();
	return true;
}
void QOpenGLObject::loadTrianleIndex(QString index1, QString index2, QString index3)
{
	QStringList indices1=index1.split("/");
	QStringList indices2=index2.split("/");
	QStringList indices3=index3.split("/");
	positionIndexList<<indices1[0]<<indices2[0]<<indices3[0];
	texcoordIndexList<<indices1[1]<<indices2[1]<<indices3[1];
	normalIndexList<<indices1[2]<<indices2[2]<<indices3[2];
}
void QOpenGLObject::loadTrianleArray()
{
	int length=positionIndexList.count();
	this->initializeVertices(length, length);
	for(int i=0; i<length; i++)
	{
		this->addTrianleVertex(i);
		this->addIndex(vertexIndex++);
	}
}
void QOpenGLObject::addTrianleVertex(int i)
{
	GLuint n=normalIndexList[i].toInt()-1;
	GLuint p=positionIndexList[i].toInt()-1;
	GLuint t=texcoordIndexList[i].toInt()-1;
	GLfloat tx=texcoordList[t*2+0].toFloat();
	GLfloat ty=texcoordList[t*2+1].toFloat();
	GLfloat px=positionList[p*3+0].toFloat();
	GLfloat py=positionList[p*3+1].toFloat();
	GLfloat pz=positionList[p*3+2].toFloat();
	GLfloat nx=normalList[n*3+0].toFloat();
	GLfloat ny=normalList[n*3+1].toFloat();
	GLfloat nz=normalList[n*3+2].toFloat();
	this->addPosition(px, py, pz);
	this->addNormal(nx, ny, nz);
	this->addTexcoord(tx, ty);
}
void QOpenGLObject::loadCube()
{
	typedef QVector3D V;
	typedef QVector2D T;
	typedef GLvertex G;
	QVector3D p[]=
	{
		V(1.0f,  -1.0f, 1.0f), V(1.0f,  1.0f, 1.0f), V(-1.0f,  1.0f, 1.0f), V(-1.0f,  -1.0f, 1.0f), 
		V(1.0f,  -1.0f, -1.0f), V(1.0f,  1.0f, -1.0f), V(-1.0f,  1.0f, -1.0f), V(-1.0f,  -1.0f, -1.0f)
	};
	QVector3D n[]=
	{
		V(1.0f,  0.0f, 0.0f), V(0.0f,  1.0f, 0.0f), V(0.0f,  0.0f, 1.0f),
		V(-1.0f,  0.0f, 0.0f), V(0.0f,  -1.0f, 0.0f), V(0.0f,  0.0f, -1.0f)
	};
	QVector2D t[]=
	{
		T(0.0f,  0.0f), T(0.33f, 0.0f), T(0.66f, 0.0f), T(1.0f, 0.0f), 
		T(0.0f,  0.5f), T(0.33f, 0.5f), T(0.66f, 0.5f), T(1.0f, 0.5f), 
		T(0.0f,  1.0f), T(0.33f, 1.0f), T(0.66f, 1.0f), T(1.0f, 1.0f)
	};
	GLvertex v[]=
	{
		G(p[0], n[2], t[5]), G(p[1], n[2], t[1]), G(p[2], n[2], t[0]), G(p[3], n[2], t[4]),
		G(p[7], n[5], t[6]), G(p[6], n[5], t[2]), G(p[5], n[5], t[1]), G(p[4], n[5], t[5]), 
		G(p[1], n[1], t[7]), G(p[5], n[1], t[3]), G(p[6], n[1], t[2]), G(p[2], n[1], t[6]), 
		G(p[0], n[4], t[5]), G(p[3], n[4], t[4]), G(p[7], n[4], t[8]), G(p[4], n[4], t[9]),
		G(p[0], n[0], t[9]), G(p[4], n[0], t[10]), G(p[5], n[0], t[6]), G(p[1], n[0], t[5]), 
		G(p[3], n[3], t[11]), G(p[2], n[3], t[7]), G(p[6], n[3], t[6]), G(p[7], n[3], t[10])
	};
	this->initializeVertices(6*4, 6*6);
	this->addQuad(v[0], v[1], v[2], v[3]);
	this->addQuad(v[4], v[5], v[6], v[7]);
	this->addQuad(v[8], v[9], v[10], v[11]);
	this->addQuad(v[12], v[13], v[14], v[15]);
	this->addQuad(v[16], v[17], v[18], v[19]);
	this->addQuad(v[20], v[21], v[22], v[23]);
}
