#include <QPainter>
#include "QCanvas.h"
#include <QMessageBox>

QCanvas::QCanvas(QWidget* widget) : QWidget(widget)
{
	this->widget=widget;
	this->resize(widget->size());
	this->shape.resize(this->size());
	this->basicTimer.start(15, this);
	this->bgColor=qRgb(255, 255, 255);
	this->image=QImage(size(), QImage::Format_RGB32);
	this->isImageModified=false; this->isMousePressed=false; this->clear();
	this->pen=QPen(Qt::green, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	this->marker=QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	this->progressBar=QPen(Qt::green, 10, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
}
bool QCanvas::loadImage(QString fileName)
{
	if(shape.load(fileName))
	{
		this->isImageModified=false; this->resize(shape.size);
		this->resizeImage(); this->resizeWindow(20);
		this->update(); return true;
	}
	else if(image.load(fileName))
	{
		this->isImageModified=false;
		this->resize(image.size());
		this->shape.resize(size());
		this->resizeWindow(20);
		this->shape.update();
		return true;
	}
	else return false;
}
bool QCanvas::saveImage(QString fileName, const char* fileFormat)
{
	if(tr(fileFormat)=="sky")return this->shape.save(fileName);
	else if(tr(fileFormat)=="svg")this->shape.saveAsSVGFile(fileName);
	else if(!image.save(fileName, fileFormat))return false;
	else this->isImageModified=false; return true;
}
void QCanvas::resizeWindow(int margin)
{
	this->widget->resize(QSize(image.width(), image.height()+margin));
}
void QCanvas::resizeCanvas()
{
	this->resizeImage(); this->shape.resize(size()); this->shape.update();
}
void QCanvas::resizeImage()
{
	this->image=image.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->isImageModified=false; this->image=image; this->update();
}
void QCanvas::clear()
{
	this->image.fill(bgColor);
	this->shape.clear();
	this->update();
}
void QCanvas::paintEvent(QPaintEvent* event)
{
	QRect rect=event->rect();
	QPainter painter(this);
	painter.drawImage(rect, image, rect);
	painter.setPen(pen);
	shape.drawPath(painter);
	painter.setPen(marker);
	shape.drawPoints(painter);
	shape.drawRegularity(painter);
	shape.drawLabelPoints(painter);
	painter.setPen(progressBar);
	shape.drawProgressBar(painter);
}
void QCanvas::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton) 
	{
		this->isMousePressed=true;
		this->shape=event->pos();
	}
}
void QCanvas::mouseMoveEvent(QMouseEvent* event)
{
	if(isMousePressed)
	{
		this->isImageModified=true;
		this->shape=event->pos();
	}
}
void QCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	if(isMousePressed)
	{
		this->isMousePressed=false;
		this->isImageModified=true;
		this->shape<<event->pos();
	}
}
void QCanvas::switchLineCurveMode()
{
	this->shape.switchLineCurveMode();
}
void QCanvas::setEditable()
{
	this->shape.isEditable=!shape.isEditable;
	this->shape.isUpdated=true;
}
void QCanvas::setAutoAligned()
{
	this->shape.isAutoAligned=!shape.isAutoAligned;
}
void QCanvas::critical(QString operation)
{
	QMessageBox::critical
	(
		this, "IIlegal Operation Exception",
		"Can not "+operation+" Sketch."
	);
}
void QCanvas::beautifySketch()
{
	if(shape.beautify())
	this->isImageModified=true;
	else critical("beautify");
}
void QCanvas::getCircles()
{
	this->shape.getCircles();
}
bool QCanvas::isModified()
{
	return this->isImageModified;
}
void QCanvas::timerEvent(QTimerEvent* event)
{
	if(shape.isOnUpdating())this->update();
	else if(shape.isUpdated)
	{
		this->update();
		this->shape.isUpdated=false;
	}
}
