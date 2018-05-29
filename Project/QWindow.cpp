#include "QWindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

QWindow::QWindow(QWidget* widget) : QWidget(widget)
{
 	this->resize(500, 400);
 	this->setMinimumSize(100, 100);
	this->setWindowTitle("QCanvas");
	this->canvas=new QCanvas(this);
	this->setLayout(newQBoxLayout());
}
QBoxLayout* QWindow::newQBoxLayout()
{
	QBoxLayout* VBoxLayout=new QVBoxLayout();
	VBoxLayout->setMenuBar(newQMenuBar());
	VBoxLayout->addWidget(canvas);
	VBoxLayout->setMargin(0);
	return VBoxLayout;
}
QMenuBar* QWindow::newQMenuBar()
{
	QMenuBar* menuBar=new QMenuBar(); QMenu* menu=new QMenu("File", this);
	QAction* action=menu->addAction("Open", this,  &QWindow::openImage, QKeySequence(tr("Ctrl+o")));
	action=menu->addAction("Save", this,  &QWindow::saveImage, QKeySequence(tr("Ctrl+s")));
	menuBar->addMenu(menu); menu=new QMenu("Edit", this);
	action=menu->addAction("Clear", canvas, &QCanvas::clear, QKeySequence(tr("Ctrl+c")));
	action=menu->addAction("Resize", canvas, &QCanvas::resizeCanvas, QKeySequence(tr("Ctrl+r")));
	action=menu->addAction("Get Circles", canvas, &QCanvas::getCircles, QKeySequence(tr("Ctrl+g")));
	action=menu->addAction("Beautify Sketch", canvas, &QCanvas::beautifySketch, QKeySequence(tr("Ctrl+b")));
	action=menu->addAction("is Line Mode", canvas, &QCanvas::switchLineCurveMode, QKeySequence(tr("Ctrl+w")));
	action->setCheckable(true); action->setChecked(true);
	action=menu->addAction("Editing Mode", canvas, &QCanvas::setEditable, QKeySequence(tr("Ctrl+e")));
	action->setCheckable(true); action->setChecked(false);
	action=menu->addAction("Auto-align", canvas, &QCanvas::setAutoAligned, QKeySequence(tr("Ctrl+a")));
	action->setCheckable(true); action->setChecked(true);
	menuBar->addMenu(menu); return menuBar;
}
bool QWindow::openImage()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Load Image", QDir::currentPath());
	if(fileName.isEmpty()||!this->canvas->loadImage(fileName))
	{
		QMessageBox::critical(this, "Error", "Can not load image."); return false;
	}
	return true;
}
bool QWindow::saveImage()
{
	QString fileFormat="SKY(*.sky);; SVG(*.svg);; JEPG(*.jpg)";
	QString fileName=QFileDialog::getSaveFileName(this, "Save Image", QString(), fileFormat);
	if(!fileName.isEmpty())
	{
		QStringList fileNames=fileName.split("."); 
		QByteArray fileFormat=fileNames[fileNames.size()-1].toLatin1();
		if(this->canvas->saveImage(fileName, fileFormat.data()))return true;
	}
	QMessageBox::critical(this, "Error", "Can not save image."); return false;
}
void QWindow::closeEvent(QCloseEvent* event)
{
	if(canvas->isModified()) 
	{
		QMessageBox::StandardButton button=QMessageBox::warning
		(
			this, "Close", "Save image?", QMessageBox::Save|
			QMessageBox::Discard|QMessageBox::Cancel
		);
		if(button==QMessageBox::Save&&saveImage())event->accept();
		else if(button==QMessageBox::Discard)event->accept();
		else if(button==QMessageBox::Cancel)event->ignore();
		else event->ignore();
	}
	else event->accept();
}
