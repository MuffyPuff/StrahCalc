#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include "klfbackend.h"
#include "klfpreviewbuilderthread.h"
#include "qexprtkbackend.h"
#include "symbollistview_w.h"

//#include "qtranslator.h"
#include "global.h"

//#include <QStandardItemModel>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void handleExprtkError();
//	void updatePreviewBuilderThreadInput(const double &res);
	void updatePreviewBuilderThreadInput();
	void updateExprtkInput();
	void showRealTimePreview(const QImage &preview, bool latexerror);
	void copyEqToClipboard();
	void copyResToClipboard();
//	void addNewVariable();
	void getResult(double value);

	void addVariable(const QString &name, const double &value);
	void removeVariable(const QString &name);
	void addConstant(const QString &name, const double &value);
	void removeConstant(const QString &name);

signals:
	void resultAvailable();

private:
//	QString roundFloat(double value, int count = 0);

private:
	Ui::MainWindow *ui;
	KLFPreviewBuilderThread *mPreviewBuilderThread;
	KLFBackend::klfInput input;
	KLFBackend::klfSettings settings;
	KLFBackend::klfOutput output;
	QExprtkBackend *mExprtk;
	QClipboard *clipboard;
	QPixmap pixmap;
	double rawValue;
	QString roundValue;

//	QTranslator translation;

//	QStandardItemModel *model;
	SymbolListView_w *mVarList;
	SymbolListView_w *mConstList;

//	int prec = 12;

//	bool compute();
};

#endif // MAINWINDOW_H
