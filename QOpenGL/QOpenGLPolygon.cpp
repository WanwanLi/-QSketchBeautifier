#include <array>
#include <QDebug>
#include "QPolygons.hpp"
#include "QOpenGLPolygon.h"
#define dot QVector3D::dotProduct
#define cross QVector3D::crossProduct

QOpenGLPolygon::QOpenGLPolygon(QVector<QVector3D*> quads, QVector<QColor> colors, QVector<QVector<QVector<qreal>>> coords)
{
	for(int i=0; i<quads.size(); i++)
	{
		this->setQuad(quads[i]);
		this->color=colors[i%colors.size()];
		this->translateQuad(quads[i], normal);
		this->loadPolygon(quads[i], coords[i]);
		this->translateQuad(quads[i], -2*normal);
		this->loadPolygon(quads[i], coords[i]);
		for(int j=0; j<coords[i].size(); j++)
		this->addPolygonSide(quads[i], coords[i][j], normal);
	}
	this->initializeVertices(positions.size(), normals.size()*3);
	for(int i=0; i<normals.size(); i++)
	{
		this->addVertex(i*3+0, i, i*3+0);
		this->addVertex(i*3+1, i, i*3+1);
		this->addVertex(i*3+2, i, i*3+2);
		this->addIndex(i*3+0);
		this->addIndex(i*3+1);
		this->addIndex(i*3+2);
	}
	this->createBuffers();
}
void QOpenGLPolygon::addPolygonSide(QVector3D* quad, QVector<qreal> coords, QVector3D normal)
{
	for(int i=0, size=coords.size()/2; i<size; i++)
	{
		int i0=i+0, i1=(i+1)%size;
		QVector3D translate=normal*thickness;
		qreal u0=coords[i0*2+0], v0=coords[i0*2+1];
		qreal u1=coords[i1*2+0], v1=coords[i1*2+1];
		QVector2D t00(u0, v0), t10=t00, t01(u1, v1), t11=t01;
		QVector3D p00=getPosition(u0, v0), p01=getPosition(u1, v1);
		QVector3D p10=p00+2*translate, p11=p01+2*translate;
		QVector3D normal=cross(p01-p00, p10-p00).normalized();
		this->texcoords<<t00<<t10<<t11<<t11<<t01<<t00;
		this->positions<<p00<<p10<<p11<<p11<<p01<<p00;
		this->normals<<normal<<normal; this->colors<<color<<color;
	}
}
void QOpenGLPolygon::translateQuad(QVector3D* quad, QVector3D normal)
{
	for(int i=0; i<4; i++)quad[i]+=normal*thickness;
}
void QOpenGLPolygon::addVertex(int p, int n, int t)
{
	GLfloat px=positions[p].x();
	GLfloat py=positions[p].y();
	GLfloat pz=positions[p].z();
	GLfloat nx=normals[n].x();
	GLfloat ny=normals[n].y();
	GLfloat nz=normals[n].z();
	GLfloat tx=texcoords[t].x();
	GLfloat ty=texcoords[t].y();
	this->addPosition(px, py, pz);
	this->addNormal(nx, ny, nz);
	this->addColor(colors[n]);
	this->addTexcoord(tx, ty);
}
void QOpenGLPolygon::setQuad(QVector3D* quad)
{
	this->p00=quad[0]; this->p01=quad[1];
	this->p11=quad[2]; this->p10=quad[3];
	this->normal=cross(p01-p00, p10-p00).normalized();
}
QVector3D QOpenGLPolygon::getPosition(qreal u, qreal v)
{
	QVector3D du=p10-p00, dv=p01-p00, y=normal;
	QVector3D x=dv.normalized(), z=cross(x, y);
	return p00+u*dot(du, z)*z+v*dot(dv, x)*x;
}
using namespace std;
using Point=array<qreal, 2>;
using Points=vector<Point>;
Points toPoints(vector<Points> polygons)
{
	Points points;
	for(Points polygon : polygons)
	{
		for(Point point : polygon)
		points.push_back(point);
	}
	return points;
}
void QOpenGLPolygon::loadPolygon(QVector3D* quad,  QVector<QVector<qreal>> coords)
{
	this->setQuad(quad);
	vector<Points> polygons;
	for(int i=0; i<coords.size(); i++)
	{
		Points polygon;
		for(int j=0; j<coords[i].size(); j+=2)
		{
			qreal x=coords[i][j+0];
			qreal y=coords[i][j+1];
			polygon.push_back({x, y});
		}
		polygons.push_back(polygon);
	}
	vector<Point> points=toPoints(polygons);
	vector<int> indices=QPolygons::triangulate<int>(polygons);
	for(int i=0; i<indices.size(); i+=3)
	{
		Point p0=points[indices[i+0]];
		Point p1=points[indices[i+1]];
		Point p2=points[indices[i+2]];
		this->texcoords<<QVector2D(p0[0], p0[1]);
		this->texcoords<<QVector2D(p1[0], p1[1]);
		this->texcoords<<QVector2D(p2[0], p2[1]);
		this->positions<<getPosition(p0[0], p0[1]);
		this->positions<<getPosition(p1[0], p1[1]);
		this->positions<<getPosition(p2[0], p2[1]);
		this->normals<<normal; this->colors<<color;
	}
}
