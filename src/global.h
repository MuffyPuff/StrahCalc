#ifndef GLOBAL_H
#define GLOBAL_H

#include "qtranslator.h"
//#include <cmath>
#include <QtMath>


#define ALL(c) (c).begin(),(c).end()
#define IN(x, c) (find(c.begin(), c.end(), x) != (c).end())
#define REP(i, n) for (int i = 0; i < (int)(n); ++i)
#define FOR(i, a, b) for (int i = (a); i <= (b); ++i)

namespace Muf
{

static QTranslator translation("en-GB");

static int prec = 12;

static QString
roundFloat(const double &value, int count = 0)
{
	//      qDebug() << "round " << double(value);
	if (count > 15) {
//		return QString::number(value * pow(10, -count), 'g', prec);
		return QString::number(value, 'g', prec);
	}
	int whole = round(value);
//	double eps = pow(10, -prec);
//	if (value > whole - eps and value < whole + eps) {
	if (qFuzzyCompare(1 + whole, 1 + value)) {
//		return QString::number(whole * pow(10, -count), 'g', prec);
		return QString::number(whole, 'g', prec);
	} else {
		return QString::number(roundFloat(value * 10,
		                                  count + 1).toDouble() * 0.1,
		                       'g', prec);
		//this->roundValue = QString::number(this->roundValue.toDouble() * 0.1);
	}
}

}


#endif // GLOBAL_H
