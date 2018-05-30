#include <meta.h>
#include <QObject>
#include <problem.h>
#include <solver/all.h>
#include <Eigen/Dense>
using namespace Eigen;
using namespace cppoptlib;

class QEnergy;
class QProblem : public QObject, public Problem<double>
{
	Q_OBJECT
	signals:
	void valueChanged(int iterations, const VectorXd& variable);

	public:
	double val;
	VectorXd grad;
	QEnergy* energy;
	QProblem(QEnergy* energy);
	double value(const VectorXd& variable);
	void getValue(const VectorXd& variable);
	void gradient(const VectorXd& variable, VectorXd& grad);
	bool callback(const Criteria<double> &criteria, const VectorXd& arg);
};
