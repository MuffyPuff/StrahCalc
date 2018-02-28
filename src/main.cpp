#include <QApplication>
#include "mainwindow.h"

int
main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setApplicationName("StrahCalc");
	QCoreApplication::setOrganizationName("rstrah");
//	QCoreApplication::setOrganizationDomain("strah.ddns.net");


	MainWindow w;
	w.setWindowTitle("StrahCalc");
	w.show();

	return a.exec();
}
