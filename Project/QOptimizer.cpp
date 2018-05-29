#include <QFile>
#include <QDebug>
#include "QDefine.h"
#include "QEnergy.h"
#include "QSketch.h"
#include "QAnalyzer.h"
#include "QOptimizer.h"
#include <QTextStream>

QString QOptimizer::fileName="QOptimizer.";
QString QOptimizer::sketchFile="QSketch.sky";
QOptimizer::QOptimizer(QThread* thread)
{
	this->moveToThread(thread);
	this->connectToThread(thread);
}
void QOptimizer::connectToThread(QThread* thread)
{
	connect(thread, &QThread::started, this, &QOptimizer::start);
	connect(this, &QOptimizer::finished, this, &QOptimizer::quit);
	connect(this, &QOptimizer::finished, thread, &QThread::quit);
}
void QOptimizer::emitValueChanged(int value, const VectorXd& variable)
{
	vec variables=QEnergy::toVector(variable);
	this->save(fileName+num(value), variables);
	emit setValue(value);
}
void QOptimizer::start()
{
	QSketch sketch;
	if(!sketch.load(sketchFile))
	{
		this->energy=NULL;
		emit finished(); return;
	}
	this->energy=new QEnergy
	(
		sketch.point2D,
		sketch.analyzer->regularity
	);
	QProblem f(energy);
	connect
	(
		&f, &QProblem::valueChanged, 
		this, &QOptimizer::valueChanged
	);
	VectorXd x=energy->variables;
	Criteria<double> criteria=Criteria<double>::defaults();
	criteria.iterations=iterations;
	GradientDescentSolver<QProblem> solver;
	solver.setStopCriteria(criteria);
	solver.minimize(f, x);
	energy->variables=x;
	emit finished();
}
void QOptimizer::valueChanged(int iterations, const VectorXd& variable)
{
	emitValueChanged(iterations, variable);
}
void QOptimizer::save(QString fileName, QVector<qreal> vector)
{
	QFile file(fileName);
	if(!file.open(QIODevice::WriteOnly))return;
	QTextStream textStream(&file);
	textStream<<num(vector.size())<<"\n";
	for(qreal x : vector)textStream<<num(x)<<"\n";
	file.close(); 
}
QVector<qreal> QOptimizer::load(QString fileName)
{
    QFile file(fileName); QVector<qreal> vector;
	if(!file.open(QIODevice::ReadOnly))return vector;
    QTextStream textStream(&file);
    int size=textStream.readLine().toInt();
    for(int i=0; i<size; i++)
    vector<<textStream.readLine().toDouble();
    file.close(); file.remove(); return vector;
}
void QOptimizer::quit()
{

}
