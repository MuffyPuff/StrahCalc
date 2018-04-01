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
	qDebug() << mPar.exprParse("-[-x -x]");
	qDebug() << mPar.exprParse("2^x");
	qDebug() << mPar.exprParse("x^(2+7*x)-(-7*x)");
	qDebug() << mPar.exprParse("");

	MainWindow w;
	w.setWindowTitle("StrahCalc");
	w.show();

	return a.exec();
}
