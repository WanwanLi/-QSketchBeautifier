#ifndef Q_DEFINE
	#define Q_DEFINE 0
	#include <QVector>
	#include <QVector2D>
	#include <QVector3D>
	#include <QVector4D>
	typedef QVector2D vec2;
	typedef QVector3D vec3;
	typedef QVector4D vec4;
	typedef QVector<int> veci;
	typedef QVector<qreal> vec;
	#define num(x) QString::number(x)
#endif
