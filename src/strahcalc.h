#ifndef STRAHCALC_H
#define STRAHCALC_H

#include <QMainWindow>
#include <QClipboard>

#include "klfbackend.h"
#include "klfpreviewbuilderthread.h"

#include "mufexprtkbackend.h"
#include "mufsymbols.h"

#include "mufsymbollistview_w.h"
#include "mufsettings_w.h"
#include "muffunctions.h"
#include "mufmenu.h"
#include "global.h"

namespace Ui
{
class StrahCalc;
}

class StrahCalc : public QMainWindow
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

	explicit StrahCalc(QWidget* parent = 0);
	~StrahCalc();

private:
//	QString         roundFloat(double value, int count = 0);
	bool            initKLF();
	bool            initExprtk();
	bool            initUI();
	bool            initMenu();
	bool            initCalcView();
	bool            initAdvCalcView();
	bool            initSymCalcView();
	bool            initSymView();
	bool            initFnView();
	bool            initSettingsView();

private slots:
	void            handleExprtkError();
//	void            updatePreviewBuilderThreadInput(const double &res);
	void            updatePreviewBuilderThreadInput();
	void            updatePreviewBuilderThreadInput_adv();
	void            updatePreviewBuilderThreadInput_plot();
	void            updateExprtkInput(const QString& input);
	void            compute();
	void            compute_adv();
	void            compute_plot();
	void            updateHistory(const QString& input);
	void            showRealTimePreview(const QImage& preview, bool latexerror);
	void            showRealTimePreview_adv(const QImage& preview, bool latexerror);
	void            showRealTimePreview_plot(const QImage& preview,
	                bool latexerror);
	void            copyEqToClipboard();
	void            copyResToClipboard();
//	void            addNewVariable();
	void            getResult(double value);
	void            clear();
	void            updateVariableDisplay();
	void            updateConstantDisplay();

	void            updateText(const QString& lang);
	void            setStatusMessage(const QString& code,
	                                 const bool& timeout = false);

	void            applySettings();
	void            openSettings();
	void            loadSettings();
	void            saveSettings();
	void            setDefaults();

	void            addVariable(const QString& name, const double& value);
	void            removeVariable(const QString& name);
	void            addConstant(const QString& name, const double& value);
	void            removeConstant(const QString& name);

signals:
	void            resultAvailable();

private:
	Ui::StrahCalc* ui;
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

	MufSymbols      mSym;


	QList<QString>  fnDirList;
	QStringList     header;

	MufSymbolListView_w* mVarList;
	MufSymbolListView_w* mConstList;

	// SETTINGS
	enum class MathMode {
		DEFAULT
	};

	MufSettings_w*  mSettings;
	QString         _lang;
	int             _timeout;
	int             _dpi;
	QColor          _color;
	QColor          _bg_color;
	MathMode        _mathmode;
};

#endif // STRAHCALC_H
