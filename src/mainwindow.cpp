#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <cmath>

#include <QDebug>
#include <QStandardPaths>
#include <QMessageBox>

#include "klfbackend.h"


using Muf::translation;


MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	if (initKLF()) {}
	if (initExprtk()) {}

	if (initUI()) {}

	if (initSymView()) {}
	if (initFnView()) {}
	if (initCalcView()) {}

	qDebug() << Muf::translation("language_code");

	// update variables when computation ends
	connect(this, &MainWindow::resultAvailable,
	        this, &MainWindow::updateVariableDisplay,
	        Qt::QueuedConnection);
	// start rendering result
	connect(this, &MainWindow::resultAvailable,
	        this, &MainWindow::updatePreviewBuilderThreadInput,
	        Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
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

	// display updated expression image
	connect(mPreviewBuilderThread,
	        &KLFPreviewBuilderThread::previewAvailable,
	        this, &MainWindow::showRealTimePreview,
	        Qt::QueuedConnection);

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
//	QMenu* menu = ui->menuBar->addMenu(Muf::translation("tools"));
////    QToolBar *toolBar = addToolBar(Muf::translation("tools"));
//	QAction* act = new QAction(Muf::translation("options"), this);
//	menu->addAction(act);
////    toolBar->addAction(act);

	initMenu();

	header.clear();
	header.append({
	        Muf::translation("calc"),
	        Muf::translation("calc_adv"),
	        Muf::translation("vars"),
	        Muf::translation("consts"),
	        Muf::translation("history")
	});

	// rename tabs
	REP(i, header.size()) {
		ui->tabWidget->setTabText(i, header.at(i));
	}

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
	        this, &MainWindow::updateExprtkInput,
	        Qt::QueuedConnection);
	// update history on return
	connect(ui->eqnInput, &QLineEdit::returnPressed,
	        this, &MainWindow::updateHistory,
	        Qt::QueuedConnection);

	// copy to clipboard
	connect(ui->clipBtnEq, &QAbstractButton::clicked,
	        this, &MainWindow::copyEqToClipboard);
	connect(ui->clipBtnRes, &QAbstractButton::clicked,
	        this, &MainWindow::copyResToClipboard);

	ui->clipBtnEq->setText(Muf::translation("copy_img"));
	ui->clipBtnRes->setText(Muf::translation("copy_res"));

	ui->eqnInput->setFocus();
	ui->statusBar->showMessage(Muf::translation("wait"));

	return true;
}

bool
MainWindow::initSymView()
{
	// var init
	mVarList   = new MufSymbolListView_w({Muf::translation("name"), Muf::translation("value")},
	                                     this);
	mConstList = new MufSymbolListView_w({Muf::translation("name"), Muf::translation("value")},
	                                     this);

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
MainWindow::openSettings()
{
	mSettings->show();
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
	ui->statusBar->showMessage(Muf::translation("img_copied"));
}

void
MainWindow::copyResToClipboard()
{
	clipboard->setText(roundValue);
	ui->statusBar->showMessage(Muf::translation("res_copied"));
}

void
MainWindow::handleExprtkError()
{
	// TODO: halt render
//	QList<exprtk::parser_error::type> errors = mExprtk->error_list;
//	for (exprtk::parser_error::type err : errors) {
//		qDebug() << "line:" << err.line_no << err.diagnostic;
//	}
	ui->statusBar->showMessage(Muf::translation("calc_err_general"));
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
		ui->statusBar->showMessage(Muf::translation("rendering"));
//		qDebug() << "input changed. displayed message";
		mPreviewBuilderThread->start();
//		qDebug() << "input changed. started";
	} else {
		ui->statusBar->showMessage(Muf::translation("done"));
	}
}

void
MainWindow::updateExprtkInput()
{
	QString input(ui->eqnInput->text());
	if (mExprtk->inputChanged(input)) {
		qDebug() << "input changed. Calculate...";
		ui->statusBar->showMessage(Muf::translation("calculating"));
		mExprtk->start();
	}
}

void MainWindow::updateHistory()
{
	if (ui->eqnInput->text().isEmpty()) {
		return;
	}
	ui->history_ls->addItem(ui->eqnInput->text());
	ui->history_ls->scrollToBottom();
}

void
MainWindow::showRealTimePreview(const QImage& preview, bool latexerror)
{
	if (latexerror) {
		ui->statusBar->showMessage(
		        Muf::translation("render_err_general"));
	} else {
		ui->statusBar->showMessage(Muf::translation("done"));
		pixmap = QPixmap::fromImage(preview);
		ui->label->setPixmap(pixmap);
		ui->label->adjustSize();
	}
}
