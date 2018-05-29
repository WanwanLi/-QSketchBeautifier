#include <QFile>
#include <QDebug>
#include "QSketch.h"
#include "QAnalyzer.h"
#include <Eigen/Dense>
using namespace Eigen;
#define toPointF(v)  QPointF(v.x(), v.y())
#define dot(x, y) vec2::dotProduct(x, y)

QString QAnalyzer::toString(int value)
{
	if(value==LOOP)return "LOOP";
	else if(value==EQUAL)return "EQ";
	else if(value==PARALLEL)return "PAR";
	else if(value==PERPENDICULAR)return "PER";
	else return "NULL";
}
int QAnalyzer::toValue(QString string)
{
	if(string=="LOOP")return LOOP;
	else if(string=="EQ")return EQUAL;
	else if(string=="PAR")return PARALLEL;
	else if(string=="PER")return PERPENDICULAR;
	else return -1;
}
int QAnalyzer::count(int value)
{
	switch(value)
	{
		case LOOP: return 2;
		case EQUAL: return 4;
		case PARALLEL: return 4;
		case PERPENDICULAR: return 4;
	}
	return 0;
}
void QAnalyzer::load(QSketch* sketch)
{
	this->sketch=sketch;
	this->path=sketch->path;
	this->point2D=sketch->point2D;
	for(int start, end, i=0; i<path.size(); i++)
	{
		if(path[i]==QSketch::MOVE)
		{
			start=i; for(i++; i<path.size()&&
			path[i]!=QSketch::MOVE; i++); i--; end=i;
			this->curves<<vec2(start, end);
			if(point2D[end]==point2D[start])
			this->loops<<vec2(start, end);
		}
	}
}
void QAnalyzer::getCircles()
{
	for(int i=0; i<chords.size(); i++)
	this->getCircle(toChordIndices(i));
	this->sketch->point2D=point2D;
}
void QAnalyzer::run()
{
	this->getChords();
	this->getRegularity();
	for(vec2 curve : curves)
	{
		int i=curve.x(), j=curve.y();
		this->getRegularity(i, j);
	}
	for(vec2 loop : loops)
	{
		this->regularity<<LOOP;
		this->regularity<<loop.x();
		this->regularity<<loop.y();
	}
}
void QAnalyzer::getChords()
{
	this->chords.clear();
	for(vec2 curve : curves)
	{
		int start=curve.x(), end=curve.y();
		for(int i=start, j=i; i<end; i++)
		{
			if(isClose(i, i+1))
			{
				for(j=i, i++; i<end&&isClose(i, i+1); i++);
				if(i==end&&isLoop(start))
				this->chords<<vec2(j, start);
				else this->chords<<vec2(j, i);
			}
		}
	}
	this->endPoints=getEndPointsOfChords(point2D);
}
QVector<vec4> QAnalyzer::getEndPointsOfChords(QVector<vec2> points)
{
	QVector<vec4> endPoints;
	for(vec2 chord : chords)
	{
		vec2 p1=points[chord.x()], p2=points[chord.y()];
		endPoints<<vec4(p1.x(), p1.y(), p2.x(), p2.y());
	}
	return endPoints;
}
QMatrix getTransformation(vec2 srcPoint0, vec2 srcPoint1, vec2 destPoint0, vec2 destPoint1)
{
	#define q0 srcPoint0
	#define q1 srcPoint1
	#define p0 destPoint0
	#define p1 destPoint1
	MatrixXd A(4, 4); VectorXd b(4);
	VectorXd X0(4); X0<<p0.x(), p0.y(), 1, 0; A.row(0)=X0; b(0)=q0.x();
	VectorXd Y0(4); Y0<<p0.y(), -p0.x(), 0, 1; A.row(1)=Y0; b(1)=q0.y();
	VectorXd X1(4); X1<<p1.x(), p1.y(), 1, 0; A.row(2)=X1; b(2)=q1.x();
	VectorXd Y1(4); Y1<<p1.y(), -p1.x(), 0, 1; A.row(3)=Y1; b(3)=q1.y();
	VectorXd h=A.jacobiSvd(ComputeThinU|ComputeThinV).solve(b);
	return QMatrix(h(0), -h(1), h(1), h(0), h(2), h(3));
	#undef q0
	#undef q1
	#undef p0
	#undef p1
}
vec2 QAnalyzer::toChordIndices(int chordIndex)
{
	int i=chordIndex;
	if(chords[i].y()>chords[i].x())return chords[i];
	else
	{
		for(vec2 loop : loops)
		{
			if(loop.x()==chords[i].y())
			return vec2(chords[i].x(), loop.y());
		}
	}
	return vec2(0, 0);
}
void QAnalyzer::updateChords(QVector<vec2>& points, QVector<vec4> endPoints)
{
	#define xy(v) vec2(v.x(), v.y())
	#define zw(v) vec2(v.z(), v.w())
	for(int i=0; i<endPoints.size(); i++)
	{
		vec2 p0=xy(this->endPoints[i]), p1=zw(this->endPoints[i]);
		vec2 p2=xy(endPoints[i]), p3=zw(endPoints[i]);
		QMatrix transform=getTransformation(p2, p3, p0, p1);
		vec2 indices=toChordIndices(i);
		for(int j=indices.x()+1; j<indices.y(); j++)
		{
			QPointF point=toPointF(points[j]);
			points[j]=vec2(transform.map(point));
		}
	}
	#undef xy(v)
	#undef zw(v)
}
bool QAnalyzer::isClose(int index1, int index2)
{
	vec2 point1=point2D[index1], point2=point2D[index2];
	return point1.distanceToPoint(point2)<distanceError;
}
void QAnalyzer::save(QString fileName)
{
	QFile file(fileName); QString endl="\r\n";
	if(!file.open(QIODevice::WriteOnly))return;
	QTextStream textStream(&file);
	this->sketch->save(textStream);
	for(int i=0; i<regularity.size(); i++)
	{
		textStream<<QAnalyzer::toString(regularity[i]);
		int n=QAnalyzer::count(regularity[i]);
		for(int t=0; t<n; t++)
		textStream<<" "<<regularity[++i];
		textStream<<endl;
	}
	file.close();
}
void QAnalyzer::operator<<(QStringList& list)
{
	int value=toValue(list[0]);
	if(value<0)return;
	int n=count(value);
	this->regularity<<value;
	for(int t=1; t<=n; t++)
	this->regularity<<list[t].toInt();
}
void insert(QVector<QVector<vec2>>& pointsCluster, vec2 startPoint, vec2 endPoint)
{
	#define has(x, y) x.indexOf(y)>=0
	for(int i=0; i<pointsCluster.size(); i++)
	{
		if(has(pointsCluster[i], startPoint)&&has(pointsCluster[i], endPoint))return;
		else if(has(pointsCluster[i], startPoint))
		{
			for(int j=i+1; j<pointsCluster.size(); j++)
			{
				if(has(pointsCluster[j], endPoint))
				{
					pointsCluster[i]+=pointsCluster[j];
					pointsCluster.removeAt(j); return;
				}
			}
			pointsCluster[i]<<endPoint; return;
		}
		else if(has(pointsCluster[i], endPoint))
		{
			for(int j=i+1; j<pointsCluster.size(); j++)
			{
				if(has(pointsCluster[j], startPoint))
				{
					pointsCluster[i]+=pointsCluster[j];
					pointsCluster.removeAt(j); return;
				}
			}
			pointsCluster[i]<<startPoint; return;
		}
	}
	#undef has(x, y)
	QVector<vec2> points;
	points<<startPoint<<endPoint;
	pointsCluster<<points;
}
void draw(QString label, vec2 translation, QPainter& painter, QVector<QVector<vec2>> pointsCluster)
{
	 for(int i=0; i<pointsCluster.size(); i++)
	{
		for(int j=0; j<pointsCluster[i].size(); j++)
		{
			vec2 p=pointsCluster[i][j]+translation;
			painter.drawText(QPointF((int)p.x(), (int)p.y()), label+num(i));
		}
	}
}
void QAnalyzer::drawChords(QPainter& painter)
{
	for(vec2 chord : chords)
	{
		vec2 p1=point2D[chord.x()], p2=point2D[chord.y()];
		painter.drawLine((int)p1.x(), (int)p1.y(), (int)p2.x(), (int)p2.y());
	}
	for(vec2 point : circles)
	{
		vec2 radius=vec2(distanceError, distanceError), center=point-radius/2;
		painter.drawEllipse((int)center.x(), (int)center.y(), (int)radius.x(), (int)radius.y());
	}
}
void QAnalyzer::drawRegularity(QPainter& painter)
{
	QVector<QVector<vec2>> parallelCenters;
	QVector<QVector<vec2>> equalLenthCenters;
	for(int i=0; i<regularity.size(); i++)
	{
		switch(regularity[i])
		{
			case PERPENDICULAR:
			{
				 vec2 p1, p2, p3, p4, p5, p6; int len=20, r1=regularity[i+1];
				int r2=regularity[i+2], r3=regularity[i+3], r4=regularity[i+4];
				if(r2==r3){p1=point2D[r1]; p2=point2D[r2], p3=point2D[r4];}
				else if(r4==r1){p1=point2D[r3]; p2=point2D[r4], p3=point2D[r2];}
				else
				{
					p1=point2D[r1]; p2=point2D[r2]; p5=(p1+p2)/2;
					p3=point2D[r3]; p4=point2D[r4]; p6=(p3+p4)/2;
					painter.drawText(QPointF((int)p5.x(), (int)p5.y()), "+");
					painter.drawText(QPointF((int)p6.x(), (int)p6.y()), "+"); break;
				}
				vec2 a=p2+len*(p1-p2).normalized(), b=p2+len*(p3-p2).normalized();
				painter.drawLine((int)p2.x(), (int)p2.y(), (int)a.x(), (int)a.y());
				painter.drawLine((int)p2.x(), (int)p2.y(), (int)b.x(), (int)b.y());
				painter.drawText(QPointF((int)p2.x(), (int)p2.y()), "PER"); break;
			}
			case PARALLEL:
			{
				int r1=regularity[i+1], r2=regularity[i+2];
				int r3=regularity[i+3], r4=regularity[i+4];
				vec2 p1=point2D[r1], p2=point2D[r2], p5=(p1+p2)/2;
				vec2 p3=point2D[r3], p4=point2D[r4], p6=(p3+p4)/2;
				insert(parallelCenters, p5, p6); break;
			}
			case EQUAL:
			{
				int r1=regularity[i+1], r2=regularity[i+2];
				int r3=regularity[i+3], r4=regularity[i+4];
				vec2 p1=point2D[r1], p2=point2D[r2], p5=(p1+p2)/2;
				vec2 p3=point2D[r3], p4=point2D[r4], p6=(p3+p4)/2;
				insert(equalLenthCenters, p5, p6); break;
			}
		}
		draw("PAR-", vec2(0, -5), painter, parallelCenters);
		draw("EQ-", vec2(0, 5), painter, equalLenthCenters);
		i+=count(regularity[i]);
	}
}
void QAnalyzer::getRegularity()
{
	for(int i=0; i<prev(prev(point2D.size())); iterate(i))
	{
		for(int j=next(i); j<prev(point2D.size()); iterate(j))
		{
			if(equals(i, j))
			{
				this->regularity<<EQUAL;
				this->regularity<<i<<next(i);
				this->regularity<<j<<next(j);
			}
		}
	}
}
void QAnalyzer::getRegularity(int startIndex, int endIndex)
{
	#define qBreak(k) {if(next(k)<=k)break;}
	for(int i=startIndex; i<prev(endIndex); i=next(i))
	{
		vec2 tangent=tangentAt(i);
		for(int j=next(i); j<endIndex; j=next(j))
		{
			if(isParallel(tangent, tangentAt(j)))
			{
				this->regularity<<PARALLEL;
				this->regularity<<i<<next(i);
				this->regularity<<j<<next(j);
			}
			else if(isPerpendicular(tangent, tangentAt(j)))
			{
				this->regularity<<PERPENDICULAR;
				this->regularity<<i<<next(i);
				this->regularity<<j<<next(j);
			}
			/*if(equals(i, j))
			{
				this->regularity<<EQUAL;
				this->regularity<<i<<next(i);
				this->regularity<<j<<next(j);
			}*/
			qBreak(j);
		}
		qBreak(i);
	}
	#undef qBreak(x)
}
void QAnalyzer::clear()
{
	this->loops.clear();
	this->circles.clear();
	this->curves.clear();
	this->chords.clear();
	this->regularity.clear();
}
int QAnalyzer::prev(int index)
{
	for(vec2 loop : loops)
	if(index==loop.x())
	return loop.y()-1;
	for(vec2 chord : chords)
	if(index==chord.y())
	return chord.x();
	return index-1;
}
int QAnalyzer::next(int index)
{
	for(vec2 loop : loops)
	if(index+1==loop.y())
	return loop.x();
	for(vec2 chord : chords)
	if(index==chord.x())
	return chord.y();
	return index+1;
}
void QAnalyzer::iterate(int& index)
{
	for(vec2 loop : loops)
	if(index+1==loop.y())
	{index++; break;}
	for(vec2 chord : chords)
	if(index==chord.x())
	{index=chord.y(); return;}
	index++;
}
bool QAnalyzer::isLoop(int startIndex)
{
	for(vec2 loop : loops)
	if(startIndex==loop.x())
	return true;
	return false;
}
vec2 QAnalyzer::tangentAt(int index)
{
	vec2 start=point2D[index];
	vec2 end=point2D[next(index)];
	return (end-start).normalized();
}
bool QAnalyzer::equals(int index1, int index2)
{
	qreal len1=(point2D[index1]-point2D[next(index1)]).length();
	qreal len2=(point2D[index2]-point2D[next(index2)]).length();
	return abs(1.0-(len1/len2))<equalLengthError;
}
qreal lengthOf(QVector<vec2> curve)
{
	qreal length=0;
	for(int i=0; i<curve.size()-1; i++)
	{
		length+=curve[i].distanceToPoint(curve[i+1]);
	}
	return length;
}
qreal distanceToCircle(QVector<vec2> curve, vec2 center, qreal radius)
{
	qreal distance=0; int n=curve.size();
	for(int i=0; i<n; i++)
	{
		qreal dx=curve[i].x()-center.x();
		qreal dy=curve[i].y()-center.y();
		qreal dr=sqrt(dx*dx+dy*dy);
		distance+=abs(dr-radius);
	}
	return distance/lengthOf(curve);
}
QVector<vec2> QAnalyzer::getPoints(vec2 chord)
{
	return point2D.mid(chord.x()+1, chord.y()-chord.x()-1);
}
vec2 slerp(vec2 direction1, vec2 direction2, qreal t)
{
	qreal angle=acos(dot(direction1, direction2));
	qreal a=sin((1.0-t)*angle)/sin(angle);
	qreal b=sin(t*angle)/sin(angle);
	return a*direction1+b*direction2;
}
void QAnalyzer::toCircle(QVector<vec2> args)
{
	int start=args[0].x(), end=args[0].y();
	qreal radius0=args[1].x(), radius1=args[1].y();
	vec2 center=args[2], direction0=args[3], direction1=args[4];
	for(int i=start; i<=end; i++)
	{
		qreal t=(i+0.0-start)/(end-start);
		qreal radius=(1.0-t)*radius0+t*radius1;
		vec2 direction=slerp(direction0, direction1, t);
		this->point2D[i]=center+radius*direction;
	}
}
void QAnalyzer::getCircle(vec2 chord)
{
	QVector<vec2> p=getPoints(chord);
	qreal xx=0, yy=0, xy=0, xz=0, yz=0;
	int n=p.size(); vec2 s=vec2(0, 0);
	for(int i=0; i<n; s+=p[i++]); s/=n;
	for(int i=0; i<n; i++)
	{
		qreal x=p[i].x()-s.x();
		qreal y=p[i].y()-s.y();
		qreal z=x*x+y*y;
		xx+=x*x; yy+=y*y;
		xy+=x*y; xz+=x*z; yz+=y*z;
	}
	xx/=n; yy/=n; xy/=n; xz/=n; yz/=n;
	qreal G11=sqrt(xx), G12=xy/G11;
	qreal G22=sqrt(yy-G12*G12);
	qreal D1=xz/G11, D2=(yz-D1*G12)/G22;
	qreal b=D2/G22/2, a=(D1-G12*b)/G11/2;
	vec2 center=vec2(a+s.x(), b+s.y());
	qreal radius=sqrt(a*a+b*b+xx+yy);
	qreal error=distanceToCircle(p, center, radius);
	if(error<circleFittingError)
	{
		this->alignLeftTangent(chord.x(), center);
		this->alignRightTangent(chord.y(), center);
		int mid=(chord.x()+chord.y())/2;
		vec2 direction0=point2D[chord.x()]-center;
		vec2 direction1=point2D[mid]-center;
		vec2 direction2=point2D[chord.y()]-center;
		qreal radius0=direction0.length(); direction0.normalize();
		qreal radius1=direction1.length(); direction1.normalize();
		qreal radius2=direction2.length(); direction2.normalize();
		QVector<vec2> args0;
		args0<<vec2(chord.x(), mid);
		args0<<vec2(radius0, radius1);
		args0<<center<<direction0<<direction1;
		QVector<vec2> args1;
		args1<<vec2(mid+1, chord.y());
		args1<<vec2(radius1, radius2);
		args1<<center<<direction1<<direction2;
		this->toCircle(args0); this->toCircle(args1);
		this->circles<<point2D[chord.x()];
		this->circles<<point2D[chord.y()];
		this->circles<<center;
	}
}
qreal cross2(vec2 p1, vec2 p2)
{
	return p1.x()*p2.y()-p1.y()*p2.x();
}
vec2 intersect(vec2 p1, vec2 p2, vec2 p3, vec2 p4)
{
	qreal x=cross2(p1, p2)*(p3.x()-p4.x())-(p1.x()-p2.x())*cross2(p3, p4);
	qreal y=cross2(p1, p2)*(p3.y()-p4.y())-(p1.y()-p2.y())*cross2(p3, p4);
	qreal z=(p1.x()-p2.x())*(p3.y()-p4.y())-(p1.y()-p2.y())*(p3.x()-p4.x());
	return vec2(x/z, y/z);
}
void QAnalyzer::alignLeftTangent(int index, vec2 center)
{
	vec2 p0=point2D[prev(index)], p1=point2D[index];
	vec2 dir0=(p1-p0).normalized(), dir1=vec2(dir0.y(), -dir0.x());
	vec2 c0=center, c1=c0+dir1; point2D[index]=intersect(p0, p1, c0, c1);
}
void QAnalyzer::alignRightTangent(int index, vec2 center)
{
	vec2 p0=point2D[index], p1=point2D[next(index)];
	vec2 dir0=(p1-p0).normalized(), dir1=vec2(dir0.y(), -dir0.x());
	vec2 c0=center, c1=c0+dir1; point2D[index]=intersect(p0, p1, c0, c1);
}
bool QAnalyzer::isParallel(vec2 x, vec2 y)
{
	qreal dotProduct=dot(x.normalized(), y.normalized());
	return abs(1-abs(dotProduct))<parallelError;
}
bool QAnalyzer::isPerpendicular(vec2 x, vec2 y)
{
	qreal dotProduct=dot(x.normalized(), y.normalized());
	return dotProduct*dotProduct<perpendicularError;
}
