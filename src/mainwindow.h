#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>

#include "klfbackend.h"
#include "klfpreviewbuilderthread.h"

#include "mufexprtkbackend.h"

#include "mufsymbollistview_w.h"
#include "mufsettings_w.h"
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
	enum class Status {
		Calculating,
		Rendering,
		Error,
		Waiting,
		Done
	};
	Q_ENUM(Status)

	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();

private:
//	QString         roundFloat(double value, int count = 0);
	bool            initKLF();
	bool            initExprtk();
	bool            initUI();
	bool            initMenu();
	bool            initCalcView();
	bool            initAdvCalcView();
	bool            initSymView();
	bool            initFnView();
	bool            initSettingsView();

private slots:
	void            handleExprtkError();
//	void            updatePreviewBuilderThreadInput(const double &res);
	void            updatePreviewBuilderThreadInput();
	void            updatePreviewBuilderThreadInput_adv();
	void            updateExprtkInput(const QString& input);
	void            compute();
	void            compute_adv();
	void            updateHistory(const QString& input);
	void            showRealTimePreview(const QImage& preview, bool latexerror);
	void            showRealTimePreview_adv(const QImage& preview, bool latexerror);
	void            copyEqToClipboard();
	void            copyResToClipboard();
//	void            addNewVariable();
	void            getResult(double value);
	void            clear();
	void            updateVariableDisplay();
	void            updateConstantDisplay();
	void            applySettings();
	void            updateText(const QString& lang);
	void            setStatusMessage(const QString& code,
	                                 const bool& timeout = false);
	void            openSettings();
	void            loadSettings();
	void            saveSettings();

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
	Status          status;
	QString         statusMessageCode;


	QList<QString>  fnDirList;
	QStringList     header;

	MufSymbolListView_w* mVarList;
	MufSymbolListView_w* mConstList;

	MufSettings_w*  mSettings;
	QString         _lang;
	int             _timeout;
};

#endif // MAINWINDOW_H
