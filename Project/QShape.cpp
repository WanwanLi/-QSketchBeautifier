#include "QShape.h"
#include <QDebug>

void QShape::operator=(QPoint point)
{
	this->isUpdated=true;
	if(isEditable)
	{
		if(index<0)this->getIndex(point);
		else 
		{
			this->point2D[index]=getPoint(point); this->update();
			this->removeLast(); this->updateLast();
		}
	}
}
vec2 QShape::getPoint(QPoint point)
{
	for(int i=0; i<point2D.size(); i++)
	{
		if(i!=index&&equals(point, pointAt(i)))return point2D[i];
	}
	return vec2(point);
}
void QShape::getIndex(QPoint point)
{
	for(qreal i=0, minDist; i<point2D.size(); i++)
	{
		qreal dist=vec2(point).distanceToPoint(point2D[i]);
		if(index<0||dist<minDist){minDist=dist; this->index=i;}
	}
}
void QShape::operator<<(QPoint point)
{
	this->isUpdated=true;
	if(isEditable){(*this)=point; this->index=-1; return;}
	if(is(LINE))this->addLineTo(point);
	else this->addCurveTo(point);
}
bool QShape::equals(QPoint point1, QPoint point2)
{
	if(!isAutoAligned)return (point1==point2);
	return vec2(point1).distanceToPoint(vec2(point2))<error;
}
QPoint QShape::toQPoint(vec2 point)
{
	return QPoint(point.x(), point.y());
}
void QShape::addLineTo(QPoint point)
{
	if(isStarted)
	{
		if(equals(point, last()))
		this->end(point);
		else
		{
			if(equals(point, beginPoint))
			{
				point=beginPoint;
				this->end(point);
			}
			(*this)+=point;
		}
	}
	else this->start(point);
}
void QShape::addCurveTo(QPoint point)
{
	if(isStarted)
	{
		if(equals(point, last()))
		this->end(point);
		else
		{
			if(equals(point, beginPoint))
			{
				point=beginPoint;
				this->end(point);
			}
			this->curves[curveIndex]<<point;
			this->curves[curveIndex].update(path, point2D);
			this->update();
		}
	}
	else this->start(point);
}
void QShape::start(QPoint point)
{
	this->isStarted=true;
	this->endPoint=NONE;
	this->beginPoint=point;
	QSketch::operator=(point);
	if(is(CUBIC))this->startCurveAt(point);
}
void QShape::startCurveAt(QPoint point)
{
	this->curves<<QCurve(path.size(), point);
	this->curveStartPoint=point;
	this->curveIndex++;
}
void QShape::switchLineCurveMode()
{
	if(is(LINE))
	{
		this->type=CUBIC;
		if(point2D.size()>0)
		if(!isCurveStarted())
		this->startCurveAt(last());
	}
	else
	{
		this->type=LINE;
		this->curveStartPoint=NONE;
	}
}
bool QShape::isCurveStarted()
{
	if(curveIndex<0)return false;
	else return curves[curveIndex].point2D.size()==1;
}
void QShape::end(QPoint point)
{
	this->endPoint=point;
	this->isStarted=false;
	this->curveStartPoint=NONE;
}
QPoint QShape::pointAt(int index)
{
	return toQPoint(point2D[index]);
}
QPoint QShape::last()
{
	return toQPoint(point2D.last());
}
void QShape::updateLast()
{
	int last=point2D.size()-1;
	if(index>last)this->index=last;
	if(index==last&&last<point2D.size())
	this->endPoint=pointAt(last);
}
void QShape::drawLabelPoints(QPainter& painter)
{
	painter.drawText(beginPoint-QPoint(2.4*error, 0), "begin");
	painter.drawText(endPoint+QPoint(1.2*error, 0), "end");
	painter.drawText(curveStartPoint+QPoint(1.2*error, 0), "curve");
	painter.drawEllipse(beginPoint, (int)error/2, (int)error/2);
	painter.drawEllipse(endPoint, (int)error/2, (int)error/2);
	painter.drawEllipse(curveStartPoint, (int)error/2, (int)error/2);
}
void QShape::drawPoints(QPainter& painter)
{
	if(!isEditable)return;
	for(vec2 point : point2D)
	{
		painter.drawEllipse(toQPoint(point), (int)error/3, (int)error/3);
	}
}
bool QShape::is(int type)
{
	return this->type==type;
}
void QShape::clear()
{
	QSketch::clear();
	this->curves.clear();
	this->curveIndex=-1;
	this->isStarted=false;
	this->endPoint=NONE;
	this->beginPoint=NONE;
	this->curveStartPoint=NONE;
}
