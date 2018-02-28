#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>

#include "klfbackend.h"
#include "klfpreviewbuilderthread.h"

#include "mufexprtkbackend.h"

#include "symbollistview_w.h"
#include "muffunctions.h"
#include "mufmenu.h"
#include "global.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	typedef KLFBackend::klfSettings KLFSettings;
	typedef KLFBackend::klfOutput     KLFOutput;
	typedef KLFBackend::klfInput       KLFInput;

public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();

private:
//	QString         roundFloat(double value, int count = 0);
	bool            initKLF();
	bool            initExprtk();
	bool            initUI();
	bool            initMenu();
	bool            initCalcView();
	bool            initSymView();
	bool            initFnView();

private slots:
	void            handleExprtkError();
//	void            updatePreviewBuilderThreadInput(const double &res);
	void            updatePreviewBuilderThreadInput();
	void            updateExprtkInput();
	void            updateHistory();
	void            showRealTimePreview(const QImage& preview, bool latexerror);
	void            copyEqToClipboard();
	void            copyResToClipboard();
//	void            addNewVariable();
	void            getResult(double value);
	void            clear();
	void            updateVariableDisplay();
	void            updateConstantDisplay();
	void            openSettings();

	void            addVariable(const QString& name, const double& value);
	void            removeVariable(const QString& name);
	void            addConstant(const QString& name, const double& value);
	void            removeConstant(const QString& name);

signals:
	void            resultAvailable();

private:
	Ui::MainWindow* ui;
	KLFPreviewBuilderThread* mPreviewBuilderThread;
	KLFInput        input;
	KLFSettings     settings;
	KLFOutput       output;
	MufExprtkBackend* mExprtk;
	QClipboard*     clipboard;
	QPixmap         pixmap;
	double          rawValue;
	QString         roundValue;
	MufFunctions*   mFnLoader;
	MufMenu*        mMenu;

	QList<QString>  fnDirList;
	QStringList     header;

	SymbolListView_w* mVarList;
	SymbolListView_w* mConstList;
};

#endif // MAINWINDOW_H
