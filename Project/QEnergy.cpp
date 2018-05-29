#include <QDebug>
#include "QEnergy.h"
#include "QAnalyzer.h"

QEnergy::QEnergy(QVector<vec2> point2D, veci regularity)
{
	vec variables;
	for(vec2 point : point2D)
	variables<<point.x()<<point.y();
	this->point2D=point2D;
	this->regularity=regularity;
	this->variables=toVectorXd(variables);
}
var QEnergy::distanceBetweenDirections(Vector2v dir1, Vector2v dir2)
{
	var dot=dir1.normalized().dot(dir2.normalized()); return 1-dot*dot;
}
var QEnergy::parallelEnergy(Vector2v start1, Vector2v end1, Vector2v start2, Vector2v end2)
{
	return distanceBetweenDirections(end1-start1, end2-start2);
}
var QEnergy::equalLengthEnergy(Vector2v start1, Vector2v end1, Vector2v start2, Vector2v end2)
{
	var length1= (end1-start1).norm();
	var length2=(end2-start2).norm();
	return (length1-length2)*(length1-length2);
}
var QEnergy::weight(int type)
{
	#define R QAnalyzer
	switch(type)
	{
		case R::PARALLEL: return 2;
		case R::EQUAL: return 0.002;
		case R::PERPENDICULAR: return 2;
	}
	return 1;
	#undef R
}
var QEnergy::totalEnergy(VectorXv variables)
{
	var energy=0;
	for(int i=0; i<regularity.size(); i++)
	{
		#define R QAnalyzer
		#define t(x) regularity[i+x]
		#define pointOf(x) variables.segment((x)*2, 2)
		switch(t(0))
		{
			case R::PARALLEL: energy+=weight(t(0))*parallelEnergy(pointOf(t(1)), pointOf(t(2)), pointOf(t(3)), pointOf(t(4))); break;
			case R::EQUAL: energy+=weight(t(0))*equalLengthEnergy(pointOf(t(1)), pointOf(t(2)), pointOf(t(3)), pointOf(t(4))); break;
			case R::PERPENDICULAR: energy+=weight(t(0))*(1-parallelEnergy(pointOf(t(1)), pointOf(t(2)), pointOf(t(3)), pointOf(t(4)))); ; break;
		}
		i+=R::count(t(0));
		#undef t(x)
		#undef R
	}
	return energy;
}
VectorXd QEnergy::toVectorXd(vec vector)
{
	std::vector<double> stdVector=vector.toStdVector();
	return Map<VectorXd>(stdVector.data(), stdVector.size());
}
vec QEnergy::toVector(VectorXd vector)
{
	vec result; for(int i=0; i<vector.size(); i++)result<<vector[i]; return result;
}
