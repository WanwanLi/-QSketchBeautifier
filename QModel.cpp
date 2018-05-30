#include <QDir>
#include <QFile>
#include "QModel.h"
#include "QSketch.h"
#define sign(x) (x>=0?1:-1)
#define dot vec3::dotProduct
#define cross vec3::crossProduct

QModel::QModel(veci path, vec point4D, veci holes, QVector<vec3*> quads)
{
	for(int i=0, j=0; i<path.size(); i++)
	{
		if(path[i]==QSketch::MOVE)
		{
			vec coordinates;
			qreal x=point4D[j++], y=point4D[j++];
			qreal z=point4D[j++], w=point4D[j++];
			vec3 point0=vec3(x, y, z), point1, point2, point3;
			this->addPoint(coordinates, point0, quads[w]);
			for(i++; i<path.size()&&path[i]!=QSketch::MOVE; i++)
			{
				if(path[i]==QSketch::LINE)
				{
					x=point4D[j++]; y=point4D[j++];
					z=point4D[j++]; w=point4D[j++];
					vec3 point=vec3(x, y, z); point0=point;
					this->addPoint(coordinates, point, quads[w]);
				}
				else if(path[i]==QSketch::CUBIC)
				{
					x=point4D[j++]; y=point4D[j++];
					z=point4D[j++]; w=point4D[j++];
					point1=vec3(x, y, z);
					x=point4D[j++]; y=point4D[j++];
					z=point4D[j++]; w=point4D[j++];
					point2=vec3(x, y, z);
					x=point4D[j++]; y=point4D[j++];
					z=point4D[j++]; w=point4D[j++];
					point3=vec3(x, y, z);
					this->addCurve(coordinates,
					new vec3[4]{point0, point1,
					point2, point3}, quads[w]);
					point0=point3;
				}
			}
			QVector<vec> coords;
			coords<<coordinates;
			this->coords<<coords;
			this->quads<<quads[w];
			if(i<path.size())i--;
		}
	}
	for(int i=0; i<holes.size(); i+=2)
	{
		int dest=holes[i+0], src=holes[i+1];
		this->coords[dest]<<coords[src][0];
		this->coords[src][0].clear();
	}
	QVector<QVector<vec>> newCoords;
	QVector<vec3*> newQuads;
	for(int i=0; i<this->coords.size(); i++)
	{
		if(this->coords[i].size()==1)
		if(this->coords[i][0].size()<6)continue;
		newCoords<<this->coords[i];
		newQuads<<this->quads[i];
	}
	this->coords=newCoords;
	this->quads=newQuads;
}
qreal max(vec vector)
{
	qreal max=vector[0];
	for(qreal v : vector)
	max=std::max(max, v);
	return max;
}
vec2 min(vec2 a, vec2 b)
{
	return vec2(std::min(a.x(), b.x()), std::min(a.y(), b.y()));
}
vec2 max(vec2 a, vec2 b)
{
	return vec2(std::max(a.x(), b.x()), std::max(a.y(), b.y()));
}
void QModel::scaleCoordinates()
{
	vec scales;
	for(vec3* quad : quads)
	{
		vec3 p00=quad[0], p01=quad[1];
		scales<<(p01-p00).length();
	}
	qreal maxScale=max(scales);
	this->transforms.clear();
	for(int i=0; i<scales.size(); i++)
	{
		qreal sx=scales[i]/maxScale;
		qreal sy=sx*aspectRatio(i);
		QVector<QTransform> transforms;
		transforms<<QTransform::fromScale(sx, sy);
		this->transforms<<transforms;
	}
	this->update();
}
vec4 getBoundingBox(vec coords)
{
	#define coordAt(x) vec2(coords[(x)*2+0], coords[(x)*2+1])
	vec2 minCoord=coordAt(0), maxCoord=coordAt(0);
	for(int i=0; i<coords.size()/2; i++)
	{
		minCoord=min(minCoord, coordAt(i));
		maxCoord=max(maxCoord, coordAt(i));
	}
	return vec4
	(
			minCoord.x(), minCoord.y(),
			maxCoord.x(), maxCoord.y()
	);
	#undef coordAt(x)
}
#define widthAt(k) (qreal)(boxes[k].z()-boxes[k].x())
#define heightAt(k) (qreal)(boxes[k].w()-boxes[k].y())
QVector<vec2> QModel::getPlacement(qreal& maxWidth)
{
	QVector<vec2> placement;
	qreal t=widthAt(0)/10, x=t, y=t;
	int column=std::ceil(std::sqrt(boxes.size()));
	for(int i=0, row=0, col=0; i<boxes.size(); i++)
	{
			int r=i/column, c=i%column;
			if(r>row)
			{
				qreal maxHeight=0.0;
				for(int j=row*column; j<r*column; j++)
				maxHeight=std::max(maxHeight, heightAt(j));
				x=t; y+=maxHeight+t; col=0; row++;
			}
			if(c>col){x+=widthAt(i-1)+t; col++;}
			maxWidth=std::max(maxWidth, x+widthAt(i)+t);
			placement<<vec2(x, y);
	}
	return placement;
}
vec autoAlignedCurve(vec curve, QTransform& transform, int index)
{
	#define pointAt(x) vec2(curve[x*2+0], curve[x*2+1])
	vec2 start=pointAt(index), end=pointAt(index+1);
	vec2 direction=(end-start).normalized();
	qreal rotate=-sign(direction.y())*acos(direction.x());
	QTransform t; t.translate(start.x(), start.y());
	t.rotateRadians(rotate); t.translate(-start.x(), -start.y());
	for(int i=0; i<curve.size(); i+=2)
	t.map(curve[i+0], curve[i+1], &curve[i+0], &curve[i+1]);
	transform=t; return curve;
	#undef pointAt(x)
}
#undef widthAt(k)
#undef heightAt(k)
qreal areaOf(vec4 box)
{
	qreal width=box.z()-box.x();
	qreal height=box.w()-box.y();
	if(height>=width)
	return height*height;
	else return height*width;
}
void QModel::rotateCoordinates()
{
	this->boxes.clear();
	this->transforms.clear();
	for(auto coord : coords)
	{
		vec curve=coord[0];
		vec4 minBox=getBoundingBox(curve);
		QTransform finalTransform, transform;
		for(int i=0; i<curve.size()/2; i++)
		{
			curve=autoAlignedCurve(coord[0], transform, i);
			vec4 box=getBoundingBox(curve);
			if(areaOf(box)<areaOf(minBox))
			{
				minBox=box;
				finalTransform=transform;
			}
		}
		this->boxes<<minBox;
		QVector<QTransform> transforms;
		transforms<<finalTransform;
		this->transforms<<transforms;
	}
	this->update();
}
void QModel::flatpack()
{
	qreal width=0, dx, dy, sx, sy;
	this->scaleCoordinates();
	this->rotateCoordinates();
	auto placement=getPlacement(width);
	this->transforms.clear();
	for(int i=0; i<quads.size(); i++)
	{
		QTransform t1;
		dx=-boxes[i].x();
		dy=-boxes[i].y();
		t1.translate(dx, dy);
		QTransform t2;
		dx=placement[i].x();
		dy=placement[i].y();
		t2.translate(dx, dy);
		QTransform t3;
		sx=outputSize/width;
		t3.scale(sx, sx);
		QVector<QTransform> transform;
		transform<<t1<<t2<<t3;
		this->transforms<<transform;
	}
	this->update();
}
void QModel::update()
{
	for(int i=0; i<coords.size(); i++)
	{
		for(int j=0; j<coords[i].size(); j++)
		{
			for(int k=0; k<coords[i][j].size(); k+=2)
			{
				qreal u=coords[i][j][k+0];
				qreal v=coords[i][j][k+1];
				for(auto transform : transforms[i])
				{
					qreal u1, v1;
					transform.map
					(u, v, &u1, &v1);
					u=u1; v=v1;
				}
				this->coords[i][j][k+0]=u;
				this->coords[i][j][k+1]=v;
			}
		}
	}
}
void QModel::save(QString fileName)
{
	QFile file(fileName); QString endl="\r\n";
	if(!file.open(QIODevice::WriteOnly))return;
	QTextStream textStream(&file);
	QString size=num(outputSize);
	textStream<<"s "<<size<<" "<<size<<endl;
	for(int i=0; i<coords.size(); i++)
	{
		for(int j=0; j<coords[i].size(); j++)
		{
			for(int k=0; k<coords[i][j].size(); k+=2)
			{
				textStream<<(k==0?"m ":"l ");
				qreal u=coords[i][j][k+0];
				qreal v=coords[i][j][k+1];
				textStream<<num((int)(u))<<" ";
				textStream<<num((int)(v))<<endl;
			}
		}
	}
	file.close();
}
void QModel::saveAsSVGFile(QString fileName)
{
	QFile file(fileName); QString endl="\r\n";
	if(!file.open(QIODevice::WriteOnly))return;
	QString svgStart="<svg height=\"1000\" width=\"1000\" ";
	svgStart+="version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">";
	QString svgEnd="\" fill=\"none\" stroke=\"blue\" stroke-width=\"1\"/></svg>";
	QTextStream textStream(&file); textStream<<svgStart<<endl; textStream<<"<path d=\""<<endl;
	for(int i=0; i<coords.size(); i++)
	{
		for(int j=0; j<coords[i].size(); j++)
		{
			for(int k=0; k<coords[i][j].size(); k+=2)
			{
				textStream<<(k==0?"M ":"L ");
				qreal u=coords[i][j][k+0];
				qreal v=coords[i][j][k+1];
				textStream<<num((int)(u))<<" ";
				textStream<<num((int)(v))<<endl;
			}
			textStream<<" Z ";
		}
	}
	textStream<<svgEnd<<endl;
	file.close();
}
qreal QModel::aspectRatio(int quadIndex)
{
	int i=quadIndex;
	vec3 p00=quads[i][0];
	vec3 p01=quads[i][1];
	vec3 p10=quads[i][3];
	vec3 du=p10-p00, dv=p01-p00;
	vec3 y=cross(du, dv).normalized();
	vec3 x=dv.normalized(), z=cross(x, y);
	qreal width=dv.length();
	qreal height=dot(du, z);
	return width/height;
}
void QModel::addPoint(vec& coordinates, vec3 point, vec3* quad)
{
	vec3 p00=quad[0],  p01=quad[1], p10=quad[3];
	vec3 dr=point-p00, du=p10-p00, dv=p01-p00;
	vec3 y=cross(du, dv).normalized();
	vec3 x=dv.normalized(), z=cross(x, y);
	coordinates<<dot(dr, z)/dot(du, z)<<dot(dr, x)/dot(dv, x);
}
void QModel::addCurve(vec& coordinates, vec3* ctrlPoints,  vec3* quad)
{
	qreal totalLength=0.0; 
	for(int i=1; i<4; i++)
	{
		totalLength+=(ctrlPoints[i]-ctrlPoints[i-1]).length();
	}
	int size=totalLength/curveLength;
	size=size<10?10:size; qreal dt=1.0/(size-1);
	for(int i=0; i<size; i++)
	{
		vec3 point=pointAt(ctrlPoints, i*dt);
		this->addPoint(coordinates, point, quad);
	}
}
vec3 QModel::pointAt(vec3* ctrlPoints, qreal t)
{
	const int degree=3;
	vec3* B=new vec3[degree+1];
	for(int i=0; i<=degree; B[i]=ctrlPoints[i++]);
	for(int i=1; i<=degree; i++) 
	{	
		for(int j=0; j<=degree-i; j++)
		{
	    		B[j]=(1.0-t)*B[j]+t*B[j+1];
		}
	}
	return B[0];
}
