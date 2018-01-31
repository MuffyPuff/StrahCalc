#ifndef GLOBAL_H
#define GLOBAL_H

#include "qtranslator.h"
//#include <cmath>
#include <QtMath>
namespace
{

QTranslator translation("en-GB");

int prec = 12;

QString
roundFloat(double value, int count = 0)
{
	//      qDebug() << "round " << double(value);
	if (count > 15) {
//		return QString::number(value * pow(10, -count), 'g', prec);
		return QString::number(value, 'g', prec);
	}
	int whole = round(value);
	double eps = pow(10, -prec);
	if (value > whole - eps and value < whole + eps) {
//		return QString::number(whole * pow(10, -count), 'g', prec);
		return QString::number(whole, 'g', prec);
	} else {
		return QString::number(roundFloat(value * 10, count + 1).toDouble() * 0.1,
		                       'g', prec);
		//this->roundValue = QString::number(this->roundValue.toDouble() * 0.1);
	}
}

}


#endif // GLOBAL_H
