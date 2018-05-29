#include <QFile>
#include <QDebug>
#include "QSketch.h"
#include "QAnalyzer.h"
#include "QOptimizer.h"

QSketch::QSketch()
{
	this->analyzer=new QAnalyzer();
	this->beautifier=new QThread();
	this->optimizer=newQOptimizer(beautifier, 600);
}
QSketch::QSketch(QSize size)
{
	QSketch::QSketch();
	this->size=size;
}
QOptimizer* QSketch::newQOptimizer(QThread* thread, int iterations)
{
	QOptimizer* optimizer=new QOptimizer(thread);
	connect(optimizer, &QOptimizer::setValue, this, &QSketch::setValue);
	connect(optimizer, &QOptimizer::finished, this, &QSketch::finished);
	optimizer->iterations=iterations; return optimizer;
}
void QSketch::setValue(int value)
{
	QString fileName=QOptimizer::fileName+num(value);
	vec variables=QOptimizer::load(fileName);
	if(variables.isEmpty())return;
	for(int i=0; i<variables.size()/2; i++)
	{
		qreal x=variables[i*2+0];
		qreal y=variables[i*2+1];
		this->point2D[i]=vec2(x, y);
	}
	this->iterations=value; this->copySamePoints();
	QVector<vec4> endPoints=analyzer->getEndPointsOfChords(point2D);
	this->analyzer->updateChords(point2D, endPoints); this->update();
}
void QSketch::copySamePoints()
{
	for(vec2 loop : analyzer->loops)
	{
		int start=loop.x(), end=loop.y();
		this->point2D[end]=this->point2D[start];
	}
}
void QSketch::finished()
{
	this->iterations=0;
}
bool QSketch::load(QString fileName)
{
	QFile file(fileName); QStringList line;
	if(!file.open(QIODevice::ReadOnly))return false;	
	QTextStream textStream(&file); 
	bool isNotValidSketch=true;
	while(!textStream.atEnd())
	{
		line=textStream.readLine().split(" ");
		if(line[0]=="#"||line[0]=="//")continue;
		if(line[0]=="s"&&line.size()>=3)
		{
			int w=line[1].toInt();
			int h=line[2].toInt();
			this->size=QSize(w, h);
			isNotValidSketch=false;
			this->clear();
		}
		if(isNotValidSketch)return false;
		if(line[0]=="m")
		{
			int x=line[1].toInt();
			int y=line[2].toInt();
			this->path<<MOVE;
			this->point2D<<vec2(x, y);
			this->moveTo(x, y);
		}
		else if(line[0]=="l")
		{
			int x=line[1].toInt();
			int y=line[2].toInt();
			this->path<<LINE;
			this->point2D<<vec2(x, y);
			this->lineTo(x, y);
		}
		else if(line[0]=="c")
		{
			int c1=line[1].toInt();
			int c2=line[2].toInt();
			int c3=line[3].toInt();
			int c4=line[4].toInt();
			int c5=line[5].toInt();
			int c6=line[6].toInt();
			this->path<<CUBIC<<CUBIC<<CUBIC;
			this->point2D<<vec2(c1, c2);
			this->point2D<<vec2(c3, c4);
			this->point2D<<vec2(c5, c6);
			this->cubicTo(c1, c2, c3, c4, c5, c6);
		}
		else (*this->analyzer)<<line;
	}
	return isNotValidSketch?false:true;
}
void QSketch::save(QTextStream& textStream)
{
	QString endl="\r\n";
	textStream<<"s "<<size.width()<<" ";
	textStream<<size.height()<<endl;
	for(int i=0, j=0; i<path.size(); i++)
	{
		if(path[i]==MOVE)
		{
			vec2 point=point2D[j++];
			textStream<<"m "<<(int)point.x();
			textStream<<" "<<(int)point.y()<<endl;
		}
		else if(path[i]==LINE)
		{
			vec2 point=point2D[j++];
			textStream<<"l "<<(int)point.x();
			textStream<<" "<<(int)point.y()<<endl;
		}
		else if(path[i]==CUBIC)
		{
			vec2 p1=point2D[j++], p2=point2D[j++], p3=point2D[j++];
			textStream<<"c "<<(int)p1.x()<<" "<<(int)p1.y(); i++;
			textStream<<" "<<(int)p2.x()<<" "<<(int)p2.y(); i++;
			textStream<<" "<<(int)p3.x()<<" "<<(int)p3.y()<<endl;
		}
	}
}
void QSketch::saveAsSVGFile(QString fileName)
{
	QFile file(fileName); QString endl="\r\n";
	if(!file.open(QIODevice::WriteOnly))return;
	QString svgStart="<svg height=\"1000\" width=\"1000\" ";
	svgStart+="version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">";
	QString svgEnd="\" fill=\"none\" stroke=\"blue\" stroke-width=\"1\"/></svg>";
	QTextStream textStream(&file); textStream<<svgStart<<endl; textStream<<"<path d=\""<<endl;
	for(int i=0, j=0; i<path.size(); i++)
	{
		if(path[i]==MOVE)
		{
			vec2 point=point2D[j++];
			textStream<<"M "<<(int)point.x();
			textStream<<" "<<(int)point.y()<<endl;
		}
		else if(path[i]==LINE)
		{
			vec2 point=point2D[j++];
			textStream<<"L "<<(int)point.x();
			textStream<<" "<<(int)point.y()<<endl;
		}
		else if(path[i]==CUBIC)
		{
			vec2 p1=point2D[j++], p2=point2D[j++], p3=point2D[j++];
			textStream<<"C "<<(int)p1.x()<<" "<<(int)p1.y(); i++;
			textStream<<" "<<(int)p2.x()<<" "<<(int)p2.y(); i++;
			textStream<<" "<<(int)p3.x()<<" "<<(int)p3.y()<<endl;
		}
		if(i+1>=path.size()||path[i+1]==MOVE)textStream<<" Z ";
	}
	textStream<<svgEnd<<endl;
	file.close();
}
bool QSketch::save(QString fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return false;
	QTextStream textStream(&file); 
	this->save(textStream); file.close(); return true;
}
void QSketch::operator=(QPoint point)
{
	this->path<<MOVE;
	this->moveTo(point);
	this->point2D<<vec2(point);
}
void QSketch::operator+=(QPoint point)
{
	this->path<<LINE;
	this->lineTo(point);
	this->point2D<<vec2(point);
}
vec2 devide(QSize x, QSize y)
{
	return vec2((x.width()+0.0)/y.width(), (x.height()+0.0)/y.height());
}
void QSketch::resize(QSize size)
{
	vec2 scale=devide(size, this->size);
	for(vec2& p : point2D)p*=scale; this->size=size;
}
void QSketch::drawPath(QPainter& painter)
{
	painter.drawPath(painterPath);
}
bool QSketch::isOnUpdating()
{
	return this->beautifier->isRunning();
}
void QSketch::update()
{
	this->isUpdated=true;
	this->painterPath=QPainterPath();
	for(int i=0, j=0; i<path.size(); i++)
	{
		if(path[i]==MOVE){vec2 p=point2D[j++]; this->moveTo(p.x(), p.y());}
		else if(path[i]==LINE){vec2 p=point2D[j++]; this->lineTo(p.x(), p.y());}
		else if(path[i]==CUBIC)
		{
			vec2 p1=point2D[j++], p2=point2D[j++], p3=point2D[j++]; i+=2;
			this->cubicTo(p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y());
		}
	}
}
bool QSketch::beautify()
{
	this->analyzer->clear();
	this->analyzer->load(this);
	this->analyzer->run();
	this->analyzer->save(fileName);
	this->isUpdated=true;
	if(beautifier->isRunning())return false;
	this->beautifier->start(); return true;
}
void QSketch::getCircles()
{
	this->analyzer->point2D=point2D;
	this->analyzer->getCircles();
	this->copySamePoints();
	this->update();
	this->isUpdated=true;
}
void QSketch::drawRegularity(QPainter& painter)
{
	this->analyzer->drawChords(painter);
	this->analyzer->drawRegularity(painter);
}
void QSketch::removeLast()
{
	if(path.size()==2)
	{
		if(point2D[0]==point2D[1])this->clear();
	}
	else if(path.size()>2)
	{
		int last=path.size()-1;
		if(point2D[last]==point2D[last-1])
		{
			this->path.removeLast();
			this->point2D.removeLast();
		}
	}
}
void QSketch::moveTo(qreal x, qreal y )
{
	this->painterPath.moveTo(x, y);
}
void QSketch::moveTo(QPoint point)
{
	this->moveTo(point.x(), point.y());
}
void QSketch::lineTo(qreal x, qreal y )
{
	this->painterPath.lineTo(x, y);
}
void QSketch::lineTo(QPoint point)
{
	this->lineTo(point.x(), point.y());
}
void QSketch::cubicTo(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3)
{
	this->painterPath.cubicTo(x1, y1, x2, y2, x3, y3);
}
void QSketch::drawProgressBar(QPainter& painter)
{
	qreal t=iterations, w=size.width();
	qreal m=optimizer->iterations;
	painter.drawLine(0, 0, w*t/m, 0);
}
void QSketch::clear()
{
	this->path.clear(); 
	this->point2D.clear();
	this->analyzer->clear();
	this->painterPath=QPainterPath();
}
