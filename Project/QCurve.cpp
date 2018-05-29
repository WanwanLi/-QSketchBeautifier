#include "QCurve.h"
#include "QSketch.h"

QCurve::QCurve(int startIndex, QPoint point)
{
	this->startIndex=startIndex;
	(*this)<<point;
}
void QCurve::operator<<(QPoint point)
{
	this->point2D<<vec2(point);
}
void QCurve::setPath(veci& path, int index, int value)
{
	int i=startIndex+index;
	if(i>=path.size())path<<value;
	else path[i]=value;
}
void QCurve::setPoint2D(QVector<vec2>& point2D, int index, vec2 point)
{
	int i=startIndex+index;
	if(i>=point2D.size())point2D<<point;
	else point2D[i]=point;
}
void QCurve::update(veci& path, QVector<vec2>& point2D)
{
	/*if(this->point2D.size()==2)
	{
		path<<QSketch::LINE;
		point2D<<this->point2D[1];
	}
	else*/
	{
		this->updateControlPoints();
		for(int i=0; i<this->point2D.size()-1; i++)
		{
			for(int j=0; j<3; j++)
			{
				this->setPath(path, i*3+j, QSketch::CUBIC);
				this->setPoint2D(point2D, i*3+j, controlPoints[i][j]);
			}
		}
	}
}
vec2* QCurve::getControlPoints(int index)
{
	vec2* ctrlPoints=new vec2[3];
	vec2 p0=point2D[index+0];
	vec2 p1=point2D[index+1];
	qreal t=1.0/3;
	ctrlPoints[0]=(1-t)*p0+t*p1;
	ctrlPoints[1]=(1-2*t)*p0+2*t*p1;
	ctrlPoints[2]=p1;
	return ctrlPoints;
}
void QCurve::updateControlPoints()
{
	this->controlPoints.clear();
	for(int i=0; i<point2D.size()-1; i++)
	this->controlPoints<<getControlPoints(i);
}
