#include <QDebug>
#include "QEnergy.h"
#include "QProblem.h"

QProblem::QProblem(QEnergy* energy)
{
	this->energy=energy;
	this->getValue(energy->variables);
}
void QProblem::getValue(const VectorXd& variable)
{
	auto totalEnergy=[&](const VectorXv& variable)
	{ 
		return energy->totalEnergy(variable);
	};
	stan::math::gradient(totalEnergy, variable, val, grad);
}
double QProblem::value(const VectorXd& variable)
{
	this->getValue(variable); return val;
}
void QProblem::gradient(const VectorXd& variable, VectorXd &grad)
{
	grad=this->grad;
}
bool QProblem::callback(const Criteria<double> &criteria, const VectorXd& variable) 
{
	emit valueChanged(criteria.iterations, variable); return true;
}
