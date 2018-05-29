#include "QDefine.h"
#include <Eigen/Dense>
using namespace Eigen;
#include <stan/math.hpp>
using namespace stan::math;
#define Vector2v Matrix<var, 2, 1>
#define Vector3v Matrix<var, 3, 1>
#define Vector4v Matrix<var, 4, 1>
#define VectorXv Matrix<var, Dynamic, 1>
#define MatrixXv Matrix<var, Dynamic, Dynamic>

class QEnergy
{
	public:
	veci regularity;
	VectorXd variables;
	var weight(int type);
	QVector<vec2> point2D;
	var totalEnergy(VectorXv variables);
	static vec toVector(VectorXd vector);
	static VectorXd toVectorXd(vec vector);
	QEnergy(QVector<vec2> point2D, veci regularity);
	var distanceBetweenDirections(Vector2v dir1, Vector2v dir2);
	var parallelEnergy(Vector2v start1, Vector2v end1, Vector2v start2, Vector2v end2);
	var equalLengthEnergy(Vector2v start1, Vector2v end1, Vector2v start2, Vector2v end2);
};




