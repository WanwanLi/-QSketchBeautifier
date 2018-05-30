#include <QObject>
#include <QThread>
#include "QProblem.h"
#include <Eigen/Dense>
using namespace Eigen;

class QEnergy;
class QOptimizer : public QObject
{
	Q_OBJECT

	signals:
	void started();
	void finished();
	void setValue(int value);

	public slots:
	void quit();
	void start();
	void valueChanged(int iterations, const VectorXd& variable);

	public:
	int iterations;
	QEnergy* energy;
	static QString fileName;
	static QString sketchFile;
	QOptimizer(QThread* thread);
	void connectToThread(QThread* thread);
    static QVector<qreal> load(QString fileName);
    static void save(QString fileName, QVector<qreal> vector);
	void emitValueChanged(int value, const VectorXd& variable);
};
