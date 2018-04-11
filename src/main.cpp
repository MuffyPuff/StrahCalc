#include <QApplication>
#include "mainwindow.h"

#include "mufsymbols.h"
#include "mufexprparser.h"

int
main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setApplicationName("StrahCalc");
	QCoreApplication::setOrganizationName("rstrah");
	QCoreApplication::setOrganizationDomain("rokstrah.com");

	MufExprParser mPar;
//	qDebug() << mPar.exprParseSY("2-3");
//	qDebug() << mPar.exprParseSY("-[-x -x]");
//	qDebug() << mPar.exprParseSY("2^x");
//	qDebug() << mPar.exprParseSY("x^(2+7*x)-(-7*x)");
//	qDebug() << mPar.exprParseSY("x^(2+7*x)-(-x/-7)");
//	qDebug() << mPar.exprParseSY("");
//	qDebug() << mPar.exprParseRD("-[-x -x]");
//	qDebug() << mPar.exprParseRD("2^x");
//	qDebug() << mPar.exprParseRD("x^(2+7*x)-(-7*x)");
//	qDebug() << mPar.exprParseRD("x^(2+7*x)-(-x/-7)");
//	qDebug() << mPar.exprParseRD("");
//	qDebug() << mPar.exprParseTD("-[-x -x]");
//	qDebug() << mPar.exprParseTD("2^x");
//	qDebug() << mPar.exprParseTD("x^(2+7*x)-(-7*x)");
//	qDebug() << mPar.exprParseTD("x^(2+7*x)-(-x/-7)");
//	qDebug() << mPar.exprParseTD("");

	Q_ASSERT(mPar("x") == "{x}");
	Q_ASSERT(mPar("0+x") == "{x}");
	Q_ASSERT(mPar("x+0") == "{x}");
	Q_ASSERT(mPar("0-x") == "{-\\left({x}\\right)}");
	Q_ASSERT(mPar("x-0") == "{x}");
	Q_ASSERT(mPar("0*x") == "{0}");
	Q_ASSERT(mPar("x*0") == "{0}");
	Q_ASSERT(mPar("1*x") == "{x}");
	Q_ASSERT(mPar("x*1") == "{x}");
	Q_ASSERT(mPar("0/x") == "{0}");
	Q_ASSERT(mPar("x/0") == "{\\infty}");
	Q_ASSERT(mPar("0^x") == "{0}");
	Q_ASSERT(mPar("1^x") == "{1}");
	Q_ASSERT(mPar("x^0") == "{1}");
	Q_ASSERT(mPar("x^1") == "{x}");
	Q_ASSERT(mPar("-x/-y") == "{\\frac{{x}}{{y}}}");
	Q_ASSERT(mPar("-x*-y") == "{{x}\\cdot{y}}");
	Q_ASSERT(mPar("x^(2+7*x)-(-x/-7)") ==
	         "{{{x}^{{2}+{{7}\\cdot{x}}}}-{\\frac{{x}}{{7}}}}");
	Q_ASSERT(mPar("t-x/y+z") == "{{{t}-{\\frac{{x}}{{y}}}}+{z}}");


	MainWindow w;
	w.setWindowTitle("StrahCalc");
	w.show();

	return a.exec();
}
