#include "strahcalc.h"
#include "ui_strahcalc.h"

#include <cmath>

#include <QDebug>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>

#include "klfbackend.h"


using Muf::translation;

bool
StrahCalc::initKLF()
{
//	input.mathmode = "\\[ ... \\]";
//	input.mathmode = "";
//	input.bypassTemplate = true;

//	input.dpi = 200;
//	ui->label->setMinimumHeight(input.dpi / 2);

//	input.bg_color = qRgba(225, 225, 225, 225);
//	input.bg_color = qRgba(0x44, 0x44, 0x44, 255);
//	input.fg_color = qRgba(255, 255, 255, 255 * 0.70);
//	input.preamble = QString("\\usepackage{amssymb,amsmath,mathrsfs}");
//	input.preamble = QString("\\usepackage{amssymb,mathtools,mathrsfs}");

	if (!KLFBackend::detectSettings(&settings, "./texlive")) {
		qDebug() << "unable to find LaTeX in default directories.";
	} else {
		qDebug() << "default settings working!";
	}

	// setup variables:
	mPreviewBuilderThread = new KLFPreviewBuilderThread(this, input, settings);

//	// display updated expression image
//	connect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
//	        this, &StrahCalc::showRealTimePreview,
//	        Qt::QueuedConnection);

	return true;
}

bool
StrahCalc::initExprtk()
{
	mExprtk = new MufExprtkBackend(this, input.latex);

	// store result from exprtk
	connect(mExprtk, &MufExprtkBackend::resultAvailable,
	        this, &StrahCalc::getResult,
	        Qt::QueuedConnection);
	// handle exprtk errors
	connect(mExprtk, &MufExprtkBackend::error,
	        this, &StrahCalc::handleExprtkError,
	        Qt::QueuedConnection);

	return true;
}

bool
StrahCalc::initUI()
{
	if (initMenu()) {}

	header.clear();
	header.append({
	        "calc",
	        "calc_adv",
	        "calc_plot",
	        "vars",
	        "consts",
	        "history"
	});

//	// rename tabs
//	REP(i, header.size()) {
//		ui->tabWidget->setTabText(i, translation(header.at(i)));
//	}

	connect(&translation, &MufTranslate::languageChanged,
	        this, &StrahCalc::updateText,
	        Qt::QueuedConnection);

	return true;
}

bool
StrahCalc::initMenu()
{
	mMenu = new MufMenu(ui->menuBar, this);

	connect(mMenu->mClear, &QAction::triggered,
	        this, &StrahCalc::clear,
	        Qt::QueuedConnection);
	connect(mMenu->mCopyImg, &QAction::triggered,
	        this, &StrahCalc::copyEqToClipboard,
	        Qt::QueuedConnection);
	connect(mMenu->mCopyRes, &QAction::triggered,
	        this, &StrahCalc::copyResToClipboard,
	        Qt::QueuedConnection);
	connect(mMenu->mSettings, &QAction::triggered,
	        this, &StrahCalc::openSettings,
	        Qt::QueuedConnection);
	connect(mMenu->mAbout, &QAction::triggered,
	[ = ]() {
		QMessageBox::about(
		        this,
		        translation("about"),
//		        "<a href=https://github.com/MuffyPuff/StrahCalc/ >"
		        "<a href=https://calc.rokstrah.com/ >"
		        "Rok Strah, Vegova, Matura 2018</a>");
	});
	connect(mMenu->mExit, &QAction::triggered,
	[ = ]() {
//		this->~StrahCalc();
		exit(0);
	});

	return true;
}

bool
StrahCalc::initCalcView()
{
	// update equation in exprtk on return
	connect(ui->eqnInput, &QLineEdit::returnPressed,
	        this, &StrahCalc::compute,
	        Qt::QueuedConnection);
//	// update history on return
//	connect(ui->eqnInput, &QLineEdit::returnPressed,
//	        this, &StrahCalc::updateHistory,
//	        Qt::QueuedConnection);

	// compute
	connect(ui->compute, &QAbstractButton::clicked,
	        this, &StrahCalc::compute,
	        Qt::QueuedConnection);
//	// update history on compute
//	connect(ui->compute, &QAbstractButton::clicked,
//	        this, &StrahCalc::updateHistory,
//	        Qt::QueuedConnection);

	// copy to clipboard
	connect(ui->clipBtnEq, &QAbstractButton::clicked,
	        this, &StrahCalc::copyEqToClipboard);
	connect(ui->clipBtnRes, &QAbstractButton::clicked,
	        this, &StrahCalc::copyResToClipboard);


//	ui->clipBtnEq->setText(Muf::translation("copy_img"));
//	ui->clipBtnRes->setText(Muf::translation("copy_res"));

	ui->eqnInput->setFocus();
//	ui->statusBar->showMessage(Muf::translation("wait"));
//	setStatusMessage("wait");

	return true;
}

bool
StrahCalc::initAdvCalcView()
{
	// compute
	connect(ui->compute_adv, &QAbstractButton::clicked,
	        this, &StrahCalc::compute_adv,
	        Qt::QueuedConnection);
//	// update history on compute
//	connect(ui->compute_adv, &QAbstractButton::clicked,
//	        this, &StrahCalc::updateHistory,
//	        Qt::QueuedConnection);

	// copy to clipboard
	connect(ui->clipBtnEq_adv, &QAbstractButton::clicked,
	        this, &StrahCalc::copyEqToClipboard);
	connect(ui->clipBtnRes_adv, &QAbstractButton::clicked,
	        this, &StrahCalc::copyResToClipboard);


//	ui->clipBtnEq->setText(Muf::translation("copy_img"));
//	ui->clipBtnRes->setText(Muf::translation("copy_res"));

	ui->eqnInput->setFocus();
//	ui->statusBar->showMessage(Muf::translation("wait"));
//	setStatusMessage("wait");

	return true;
}

bool
StrahCalc::initPlotView()
{
	// compute
	connect(ui->compute_plot, &QAbstractButton::clicked,
	        this, &StrahCalc::compute_plot,
	        Qt::QueuedConnection);
//	// update history on compute
//	connect(ui->compute_adv, &QAbstractButton::clicked,
//	        this, &StrahCalc::updateHistory,
//	        Qt::QueuedConnection);

	// copy to clipboard
	connect(ui->clipBtnEq_plot, &QAbstractButton::clicked,
	        this, &StrahCalc::copyEqToClipboard);

//	ui->clipBtnEq->setText(Muf::translation("copy_img"));
//	ui->clipBtnRes->setText(Muf::translation("copy_res"));

	ui->eqnInput->setFocus();
//	ui->statusBar->showMessage(Muf::translation("wait"));
//	setStatusMessage("wait");

	return true;
}

bool
StrahCalc::initSymView()
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
StrahCalc::initFnView()
{
	fnDirList.append(QDir::cleanPath(
	                         QDir::current().absolutePath() +
	                         "/../StrahCalc/functions/"));

	mFnLoader = new MufFunctions(fnDirList, mExprtk, this);

	return true;
}

bool
StrahCalc::initSettingsView()
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

//	// init values
//	mSettings->languages->setCurrentText(translation("language_name"));
//	mSettings->timeout->setValue(_timeout);
//	mSettings->dpi->setValue(_dpi);
//	mSettings->color->setText(_color.name());
//	mSettings->bg_color->setText(_bg_color.name());

	connect(mSettings, &MufSettings_w::accepted,
	        this, &StrahCalc::saveSettings,
	        Qt::QueuedConnection);
	connect(mSettings, &MufSettings_w::defaults,
	        this, &StrahCalc::setDefaults,
	        Qt::QueuedConnection);

	return true;
}
