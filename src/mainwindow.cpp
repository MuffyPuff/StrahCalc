#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>

#include <QDebug>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>

#include "klfbackend.h"


using Muf::translation;


MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	loadSettings();
	translation.changeLanguage(_lang);
	statusMessageCode = "wait";
	status = Status::Waiting;

	if (initKLF()) {}
	if (initExprtk()) {}

	if (initUI()) {}

	if (initSymView()) {}
	if (initFnView()) {}
	if (initCalcView()) {}
	if (initAdvCalcView()) {}
	if (initSettingsView()) {}

//	qDebug() << Muf::translation("language_code");

	// update variables when computation ends
	connect(this, &MainWindow::resultAvailable,
	        this, &MainWindow::updateVariableDisplay,
	        Qt::QueuedConnection);
//	// start rendering result
//	connect(this, &MainWindow::resultAvailable,
//	        this, &MainWindow::updatePreviewBuilderThreadInput,
//	        Qt::QueuedConnection);

	updateText(_lang);
}

MainWindow::~MainWindow()
{
	saveSettings();
	delete mPreviewBuilderThread;
	delete mExprtk;
	delete mFnLoader;
	delete mSettings;
	delete mMenu;
	delete ui;
	this->destroy();
}

bool
MainWindow::initKLF()
{
	input.mathmode = "\\[ ... \\]";
//	input.bypassTemplate = true;

	input.dpi = 150;
	ui->label->setMinimumHeight(input.dpi / 2);

//	input.bg_color = qRgba(225, 225, 225, 225);
	input.bg_color = qRgba(0x44, 0x44, 0x44, 255);
	input.fg_color = qRgba(255, 255, 255, 255 * 0.70);
	input.preamble = QString("\\usepackage{amssymb,amsmath,mathrsfs}");

	if (!KLFBackend::detectSettings(&settings, "./texlive")) {
		qDebug() << "unable to find LaTeX in default directories.";
	} else {
		qDebug() << "default settings working!";
	}

	// setup variables:
	mPreviewBuilderThread = new KLFPreviewBuilderThread(this, input, settings);

//	// display updated expression image
//	connect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
//	        this, &MainWindow::showRealTimePreview,
//	        Qt::QueuedConnection);

	return true;
}

bool
MainWindow::initExprtk()
{
	mExprtk = new MufExprtkBackend(this, input.latex);

	// store result from exprtk
	connect(mExprtk, &MufExprtkBackend::resultAvailable,
	        this, &MainWindow::getResult,
	        Qt::QueuedConnection);
	// handle exprtk errors
	connect(mExprtk, &MufExprtkBackend::error,
	        this, &MainWindow::handleExprtkError,
	        Qt::QueuedConnection);

	return true;
}

bool
MainWindow::initUI()
{
	if (initMenu()) {}

	header.clear();
	header.append({
	        "calc",
	        "calc_adv",
	        "vars",
	        "consts",
	        "history"
	});

//	// rename tabs
//	REP(i, header.size()) {
//		ui->tabWidget->setTabText(i, translation(header.at(i)));
//	}

	connect(&translation, &MufTranslate::languageChanged,
	        this, &MainWindow::updateText,
	        Qt::QueuedConnection);

	return true;
}

bool
MainWindow::initMenu()
{
	mMenu = new MufMenu(ui->menuBar, this);

	connect(mMenu->mClear, &QAction::triggered,
	        this, &MainWindow::clear,
	        Qt::QueuedConnection);
	connect(mMenu->mCopyImg, &QAction::triggered,
	        this, &MainWindow::copyEqToClipboard,
	        Qt::QueuedConnection);
	connect(mMenu->mCopyRes, &QAction::triggered,
	        this, &MainWindow::copyResToClipboard,
	        Qt::QueuedConnection);
	connect(mMenu->mSettings, &QAction::triggered,
	        this, &MainWindow::openSettings,
	        Qt::QueuedConnection);
	connect(mMenu->mAbout, &QAction::triggered,
	[ = ]() {
		QMessageBox::about(
		        this,
		        translation("about"),
		        "<a href=https://github.com/MuffyPuff/StrahCalc/ >"
		        "Rok Strah, Vegova, Matura 2018</a>");
	});
	connect(mMenu->mExit, &QAction::triggered,
	[ = ]() {
//		this->~MainWindow();
		exit(0);
	});

	return true;
}

bool
MainWindow::initCalcView()
{
	// update equation in exprtk on return
	connect(ui->eqnInput, &QLineEdit::returnPressed,
	        this, &MainWindow::compute,
	        Qt::QueuedConnection);
//	// update history on return
//	connect(ui->eqnInput, &QLineEdit::returnPressed,
//	        this, &MainWindow::updateHistory,
//	        Qt::QueuedConnection);

	// compute
	connect(ui->compute, &QAbstractButton::clicked,
	        this, &MainWindow::compute,
	        Qt::QueuedConnection);
//	// update history on compute
//	connect(ui->compute, &QAbstractButton::clicked,
//	        this, &MainWindow::updateHistory,
//	        Qt::QueuedConnection);

	// copy to clipboard
	connect(ui->clipBtnEq, &QAbstractButton::clicked,
	        this, &MainWindow::copyEqToClipboard);
	connect(ui->clipBtnRes, &QAbstractButton::clicked,
	        this, &MainWindow::copyResToClipboard);


//	ui->clipBtnEq->setText(Muf::translation("copy_img"));
//	ui->clipBtnRes->setText(Muf::translation("copy_res"));

	ui->eqnInput->setFocus();
//	ui->statusBar->showMessage(Muf::translation("wait"));
//	setStatusMessage("wait");

	return true;
}

bool
MainWindow::initAdvCalcView()
{
	// compute
	connect(ui->compute_adv, &QAbstractButton::clicked,
	        this, &MainWindow::compute_adv,
	        Qt::QueuedConnection);
//	// update history on compute
//	connect(ui->compute_adv, &QAbstractButton::clicked,
//	        this, &MainWindow::updateHistory,
//	        Qt::QueuedConnection);

	// copy to clipboard
	connect(ui->clipBtnEq_adv, &QAbstractButton::clicked,
	        this, &MainWindow::copyEqToClipboard);
	connect(ui->clipBtnRes_adv, &QAbstractButton::clicked,
	        this, &MainWindow::copyResToClipboard);


//	ui->clipBtnEq->setText(Muf::translation("copy_img"));
//	ui->clipBtnRes->setText(Muf::translation("copy_res"));

	ui->eqnInput->setFocus();
//	ui->statusBar->showMessage(Muf::translation("wait"));
//	setStatusMessage("wait");

	return true;
}

bool
MainWindow::initSymView()
{
	// var init
	mVarList   = new MufSymbolListView_w({"name", "value"}, this);
	mConstList = new MufSymbolListView_w({"name", "value"}, this);

	ui->varList_t->layout()->addWidget(mVarList);
	ui->constList_t->layout()->addWidget(mConstList);

	updateVariableDisplay();
	updateConstantDisplay();

	// add symbols from view to exprtk
	connect(mVarList, SIGNAL(addSym(const QString&, const double&)),
	        this, SLOT(addVariable(const QString&, const double&)));
	connect(mVarList, SIGNAL(remSym(const QString&)),
	        this, SLOT(removeVariable(const QString&)));
	connect(mConstList, SIGNAL(addSym(const QString&, const double&)),
	        this, SLOT(addConstant(const QString&, const double&)));
	connect(mConstList, SIGNAL(remSym(const QString&)),
	        this, SLOT(removeConstant(const QString&)));

	return true;
}

// TODO: convert functions to view
bool
MainWindow::initFnView()
{
	fnDirList.append(QDir::cleanPath(
	                         QDir::current().absolutePath() +
	                         "/../StrahCalc/functions/"));

	mFnLoader = new MufFunctions(fnDirList, mExprtk, this);

	return true;
}

bool
MainWindow::initSettingsView()
{
	mSettings = new MufSettings_w();

	QDir languageDir = QDir::cleanPath(QDir::current().absolutePath() +
	                                   "/../StrahCalc/lang/");
//	qDebug() << languageDir.absolutePath();
	for (auto& fn : languageDir.entryInfoList()) {
		if (fn.isFile() and fn.suffix() == "json") {
//			qDebug() << fn.fileName();
			QString lang = translation("language_name", fn.baseName());
			mSettings->languages->addItem(lang);
			MufTranslate::_languageList[fn.baseName()] = lang;
		}
	}

	connect(mSettings, &MufSettings_w::accepted,
	        this, &MainWindow::applySettings,
	        Qt::QueuedConnection);

	return true;
}

void
MainWindow::getResult(double value)
{
	this->rawValue = value;
	this->roundValue = Muf::roundFloat(value);
	emit resultAvailable();
}

void
MainWindow::clear()
{
	ui->eqnInput->clear();
	ui->label->clear();
	ui->history_ls->clear();
}

void
MainWindow::updateVariableDisplay()
{
	mVarList->setList(mExprtk->getVariables());
}

void
MainWindow::updateConstantDisplay()
{
	mConstList->setList(mExprtk->getConstants());
}

void
MainWindow::applySettings()
{
	_lang = MufTranslate::_languageList.key(
	                mSettings->languages->currentText());
	translation.changeLanguage(_lang);

	_timeout = mSettings->timeout->value();
}

void
MainWindow::updateText(const QString& lang)
{
	// rename tabs
	REP(i, header.size()) {
		ui->tabWidget->setTabText(i, translation(header.at(i)));
	}

	ui->clipBtnEq->setText(Muf::translation("copy_img"));
	ui->clipBtnRes->setText(Muf::translation("copy_res"));
	ui->compute->setText(Muf::translation("compute"));

	ui->clipBtnEq_adv->setText(Muf::translation("copy_img"));
	ui->clipBtnRes_adv->setText(Muf::translation("copy_res"));
	ui->compute_adv->setText(Muf::translation("compute"));

	setStatusMessage(statusMessageCode);

	mVarList->updateText(lang);
	mConstList->updateText(lang);
	mSettings->updateText(lang);
	mMenu->updateText(lang);
}

void
MainWindow::setStatusMessage(const QString& code, const bool& timeout)
{
	if (timeout == true) {
		QTimer::singleShot(_timeout, this,
		[ = ]() {
			setStatusMessage(statusMessageCode);
		});
	} else {
		statusMessageCode = code;
	}
	ui->statusBar->showMessage(translation(code));
}

void
MainWindow::openSettings()
{
	mSettings->languages->setCurrentText(translation("language_name"));
	mSettings->timeout->setValue(_timeout);

	mSettings->show();
	mSettings->raise();
	mSettings->activateWindow();
}

void
MainWindow::loadSettings()
{
	QSettings settings;
	_lang           = settings.value("ui/language_code",    "sl-SI").toString();
	_timeout        = settings.value("ui/message_timeout",  3000).toInt();
}

void
MainWindow::saveSettings()
{
	QSettings settings;
	settings.setValue("ui/language_code",           _lang);
	settings.setValue("ui/message_timeout",         _timeout);

}

void
MainWindow::addVariable(const QString& name, const double& value)
{
	mExprtk->addVariable(name, value);
}

void
MainWindow::removeVariable(const QString& name)
{
	// TODO: implement removal
	Q_UNUSED(name);
	Q_UNIMPLEMENTED();
}

void
MainWindow::addConstant(const QString& name, const double& value)
{
	mExprtk->addConstant(name, value);
}

void
MainWindow::removeConstant(const QString& name)
{
	// TODO: implement removal
	Q_UNUSED(name);
	Q_UNIMPLEMENTED();
}

void
MainWindow::copyEqToClipboard()
{
	clipboard->setPixmap(pixmap);
//	ui->statusBar->showMessage(Muf::translation("img_copied"));
	setStatusMessage("img_copied", true);
}

void
MainWindow::copyResToClipboard()
{
	clipboard->setText(roundValue);
//	ui->statusBar->showMessage(Muf::translation("res_copied"));
	setStatusMessage("res_copied", true);
}

void
MainWindow::handleExprtkError()
{
	// TODO: halt render
//	QList<exprtk::parser_error::type> errors = mExprtk->error_list;
//	for (exprtk::parser_error::type err : errors) {
//		qDebug() << "line:" << err.line_no << err.diagnostic;
//	}
//	ui->statusBar->showMessage(Muf::translation("calc_err_general"));
	setStatusMessage("calc_err_general");
	Q_UNIMPLEMENTED();
}

void
MainWindow::updatePreviewBuilderThreadInput()
{
//	if (!compute()) {
//		return;
//	}
//	this->value = res;

	// in linux, I need to reinstate the preamble when rendering. No idea why.
	input.preamble =
	        QString("\\usepackage{amssymb,amsmath}"); // add functions here \n\\DeclareMathOperator\\cis{cis}

	/**
	 * TODO: multiline
	 * TODO: strip trailing semicolon
	 * TODO: some assignment stuff
	 **/


	input.latex = ui->eqnInput->text() +
	              " = " +
	              roundValue;
	if (mPreviewBuilderThread->inputChanged(input)) {
		qDebug() << "input changed. Render...";
//		ui->statusBar->showMessage(Muf::translation("rendering"));
		setStatusMessage("rendering");
//		qDebug() << "input changed. displayed message";
		mPreviewBuilderThread->start();
//		qDebug() << "input changed. started";
	} else {
//		ui->statusBar->showMessage(Muf::translation("done"));
		setStatusMessage("done");
	}
}

void
MainWindow::updatePreviewBuilderThreadInput_adv()
{
	//      if (!compute()) {
	//              return;
	//      }
	//      this->value = res;

	// in linux, I need to reinstate the preamble when rendering. No idea why.
	input.preamble =
	        QString("\\usepackage{amssymb,amsmath}"); // add functions here \n\\DeclareMathOperator\\cis{cis}

	/**
	 * TODO: multiline
	 * TODO: strip trailing semicolon
	 * TODO: some assignment stuff
	 **/

	// TODO add parsing
	QString prefix = ui->eqnInput_adv->toPlainText().split(';').last();
	prefix = prefix.split(":=").first();
	prefix = prefix.split("+=").first();
	prefix = prefix.split("-=").first();
	prefix = prefix.split("*=").first();
	prefix = prefix.split("/=").first();
	input.latex = prefix +
	              " = " +
	              roundValue;
	if (mPreviewBuilderThread->inputChanged(input)) {
		qDebug() << "input changed. Render...";
		//              ui->statusBar->showMessage(Muf::translation("rendering"));
		setStatusMessage("rendering");
		//              qDebug() << "input changed. displayed message";
		mPreviewBuilderThread->start();
		//              qDebug() << "input changed. started";
	} else {
		//              ui->statusBar->showMessage(Muf::translation("done"));
		setStatusMessage("done");
	}
}

void
MainWindow::updateExprtkInput(const QString& input)
{
//	QString input(ui->eqnInput->text());
	if (mExprtk->inputChanged(input)) {
		qDebug() << "input changed. Calculate...";
//		ui->statusBar->showMessage(Muf::translation("calculating"));
		setStatusMessage("calculating");
		mExprtk->start();
	}
}

void
MainWindow::compute()
{
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &MainWindow::showRealTimePreview);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &MainWindow::showRealTimePreview_adv);

	disconnect(this, &MainWindow::resultAvailable,
	           this, &MainWindow::updatePreviewBuilderThreadInput);
	disconnect(this, &MainWindow::resultAvailable,
	           this, &MainWindow::updatePreviewBuilderThreadInput_adv);

	// display updated expression image
	connect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	        this, &MainWindow::showRealTimePreview,
	        Qt::QueuedConnection);
	// start rendering result
	connect(this, &MainWindow::resultAvailable,
	        this, &MainWindow::updatePreviewBuilderThreadInput,
	        Qt::QueuedConnection);
	updateExprtkInput(ui->eqnInput->text());
	updateHistory(ui->eqnInput->text());
}

void
MainWindow::compute_adv()
{
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &MainWindow::showRealTimePreview);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &MainWindow::showRealTimePreview_adv);

	disconnect(this, &MainWindow::resultAvailable,
	           this, &MainWindow::updatePreviewBuilderThreadInput);
	disconnect(this, &MainWindow::resultAvailable,
	           this, &MainWindow::updatePreviewBuilderThreadInput_adv);

	// display updated expression image
	connect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	        this, &MainWindow::showRealTimePreview_adv,
	        Qt::QueuedConnection);
	// start rendering result
	connect(this, &MainWindow::resultAvailable,
	        this, &MainWindow::updatePreviewBuilderThreadInput_adv,
	        Qt::QueuedConnection);
	updateExprtkInput(ui->eqnInput_adv->toPlainText());
	updateHistory(ui->eqnInput_adv->toPlainText());
}

void MainWindow::updateHistory(const QString& input)
{
	if (input.isEmpty()) {
		return;
	}
	ui->history_ls->addItem(input);
	ui->history_ls->scrollToBottom();
}

void
MainWindow::showRealTimePreview(const QImage& preview, bool latexerror)
{
	if (latexerror) {
//		ui->statusBar->showMessage(Muf::translation("render_err_general"));
		setStatusMessage("render_err_general");
	} else {
//		ui->statusBar->showMessage(Muf::translation("done"));
		setStatusMessage("done");
		pixmap = QPixmap::fromImage(preview);
		ui->label->setPixmap(pixmap);
		ui->label->adjustSize();
	}
}

void
MainWindow::showRealTimePreview_adv(const QImage& preview, bool latexerror)
{
	if (latexerror) {
		//              ui->statusBar->showMessage(Muf::translation("render_err_general"));
		setStatusMessage("render_err_general");
	} else {
		//              ui->statusBar->showMessage(Muf::translation("done"));
		setStatusMessage("done");
		pixmap = QPixmap::fromImage(preview);
		ui->label_adv->setPixmap(pixmap);
		ui->label_adv->adjustSize();
	}
}
