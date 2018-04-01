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
	qDebug() << mPar.exprParseSY("-[-x -x]");
	qDebug() << mPar.exprParseSY("2^x");
	qDebug() << mPar.exprParseSY("x^(2+7*x)-(-7*x)");
	qDebug() << mPar.exprParseSY("x^(2+7*x)-(-x/-7)");
	qDebug() << mPar.exprParseSY("");
	qDebug() << mPar.exprParseRD("-[-x -x]");
	qDebug() << mPar.exprParseRD("2^x");
	qDebug() << mPar.exprParseRD("x^(2+7*x)-(-7*x)");
	qDebug() << mPar.exprParseRD("x^(2+7*x)-(-x/-7)");
	qDebug() << mPar.exprParseRD("");
	qDebug() << mPar.exprParseTD("-[-x -x]");
	qDebug() << mPar.exprParseTD("2^x");
	qDebug() << mPar.exprParseTD("x^(2+7*x)-(-7*x)");
	qDebug() << mPar.exprParseTD("x^(2+7*x)-(-x/-7)");
	qDebug() << mPar.exprParseTD("");

	MainWindow w;
	w.setWindowTitle("StrahCalc");
	w.show();

	return a.exec();
}
