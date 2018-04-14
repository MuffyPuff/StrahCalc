#include "strahcalc.h"
#include "ui_strahcalc.h"

#include <cmath>

#include <QDebug>
#include <QStandardPaths>
#include <QMessageBox>
#include <QTimer>

#include "klfbackend.h"


using Muf::translation;


StrahCalc::StrahCalc(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::StrahCalc)
{
	ui->setupUi(this);

	loadSettings();
	applySettings();
	statusMessageCode = "wait";
	status = Status::Waiting;

	if (initKLF()) {}
	if (initExprtk()) {}

	if (initUI()) {}

	if (initSymView()) {}
	if (initFnView()) {}
	if (initCalcView()) {}
	if (initAdvCalcView()) {}
	if (initPlotView()) {}
	if (initSettingsView()) {}

//	qDebug() << Muf::translation("language_code");

	// update variables when computation ends
	connect(this, &StrahCalc::resultAvailable,
	        this, &StrahCalc::updateVariableDisplay,
	        Qt::QueuedConnection);
//	// start rendering result
//	connect(this, &StrahCalc::resultAvailable,
//	        this, &StrahCalc::updatePreviewBuilderThreadInput,
//	        Qt::QueuedConnection);

	updateText(_lang);
}

StrahCalc::~StrahCalc()
{
//	saveSettings();
	delete mPreviewBuilderThread;
	delete mExprtk;
	delete mFnLoader;
	delete mSettings;
	delete mMenu;
	delete ui;
	this->destroy();
}
//2^(1+((x+1)^(x+2))%3)+1


void
StrahCalc::getResult(double value)
{
	this->rawValue = value;
	this->roundValue = Muf::roundFloat(value);
	emit resultAvailable();
}

void
StrahCalc::clear()
{
	// TODO: extend
	ui->eqnInput->clear();
	ui->label->clear();
	ui->history_ls->clear();
}

void
StrahCalc::updateVariableDisplay()
{
	mVarList->setList(mExprtk->getVariables());
}

void
StrahCalc::updateConstantDisplay()
{
	mConstList->setList(mExprtk->getConstants());
}

void
StrahCalc::updateText(const QString& lang)
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

	ui->clipBtnEq_plot->setText(Muf::translation("copy_img"));
	ui->compute_plot->setText(Muf::translation("compute"));

	setStatusMessage(statusMessageCode);

	mVarList->updateText(lang);
	mConstList->updateText(lang);
	mSettings->updateText(lang);
	mMenu->updateText(lang);
}

void
StrahCalc::setStatusMessage(const QString& code, const bool& timeout)
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
StrahCalc::applySettings()
{
	translation.changeLanguage(_lang);
	input.dpi = _dpi;
	ui->label->setMinimumHeight(_dpi / 2);
	input.fg_color = _color.rgba();
	input.bg_color = _bg_color.rgba();
	ui->label->setStyleSheet("background-color: " + _bg_color.name() + ";");
	ui->label_adv->setStyleSheet("background-color: " + _bg_color.name() + ";");
	ui->label_plot->setStyleSheet("background-color: " + _bg_color.name() + ";");
	ui->label->repaint();
	qDebug() << "applied settings";
}

void
StrahCalc::openSettings()
{
	mSettings->languages->setCurrentText(translation("language_name"));
	mSettings->timeout->setValue(_timeout);
	mSettings->dpi->setValue(_dpi);
	mSettings->color->setText(_color.name());
	mSettings->bg_color->setText(_bg_color.name());

	mSettings->show();
	mSettings->raise();
	mSettings->activateWindow();
	qDebug() << "opened settings";
}

void
StrahCalc::loadSettings()
{
	QSettings settings;
//	qDebug() << settings.fileName();
	_lang           = settings.value("ui/language_code",    "sl-SI").toString();
	_timeout        = settings.value("ui/message_timeout",  3000).toInt();
	_dpi            = settings.value("label/dpi",           200).toInt();
	_color.setNamedColor(
	        settings.value(
	                "label/color",
	                QColor(0xff, 0xff, 0xff, 255 * 0.70).name()).toString());
	_bg_color.setNamedColor(
	        settings.value(
	                "label/bg_color",
	                QColor(0x44, 0x44, 0x44, 255).name()).toString());

	qDebug() << "loaded settings";
}

void
StrahCalc::saveSettings()
{
//	qDebug() << mSettings->languages->currentText();
//	qDebug() << MufTranslate::_languageList;
	_lang = MufTranslate::_languageList.key(mSettings->languages->currentText());
	_timeout = mSettings->timeout->value();
	_dpi = mSettings->dpi->value();

	QSettings settings;
	settings.setValue("ui/language_code",           _lang);
	settings.setValue("ui/message_timeout",         _timeout);
	settings.setValue("label/dpi",                  _dpi);
	if (QColor::isValidColor(mSettings->color->text())) {
		qDebug() << "valid color";
		_color.setNamedColor(mSettings->color->text());
		settings.setValue("label/color",        _color.name());
	}
	if (QColor::isValidColor(mSettings->bg_color->text())) {
		qDebug() << "valid bg color";
		_bg_color.setNamedColor(mSettings->bg_color->text());
		settings.setValue("label/bg_color",     _bg_color.name());
	}

	qDebug() << "saved settings";
	applySettings();
}

void
StrahCalc::setDefaults()
{
	QSettings settings;
	settings.clear();
	loadSettings();
	applySettings();
	mSettings->hide();
}

void
StrahCalc::addVariable(const QString& name, const double& value)
{
	mExprtk->addVariable(name, value);
}

void
StrahCalc::removeVariable(const QString& name)
{
	mExprtk->addVariable(name, 0);
	// TODO: implement removal
	Q_UNUSED(name);
	Q_UNIMPLEMENTED();
}

void
StrahCalc::addConstant(const QString& name, const double& value)
{
	mExprtk->addConstant(name, value);
}

void
StrahCalc::removeConstant(const QString& name)
{
	// TODO: implement removal
	Q_UNUSED(name);
	Q_UNIMPLEMENTED();
}

void
StrahCalc::copyEqToClipboard()
{
	clipboard->setPixmap(pixmap);
//	ui->statusBar->showMessage(Muf::translation("img_copied"));
	setStatusMessage("img_copied", true);
}

void
StrahCalc::copyResToClipboard()
{
	clipboard->setText(roundValue);
//	ui->statusBar->showMessage(Muf::translation("res_copied"));
	setStatusMessage("res_copied", true);
}

void
StrahCalc::handleExprtkError()
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
StrahCalc::updatePreviewBuilderThreadInput()
{
//	if (!compute()) {
//		return;
//	}
//	this->value = res;

	// in linux, I need to reinstate the preamble when rendering. No idea why.
	input.preamble =
	        QString("\\usepackage{amssymb,mathtools,mathrsfs}"); // add functions here \n\\DeclareMathOperator\\cis{cis}
//	        QString("\\usepackage{amssymb,amsmath,mathrsfs}"); // add functions here \n\\DeclareMathOperator\\cis{cis}

	input.mathmode = "\\[ ... \\]";
	input.bypassTemplate = false;

	input.latex = Muf::toLatex(ui->eqnInput->text(), false) +
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
StrahCalc::updatePreviewBuilderThreadInput_adv()
{
	qDebug("adv update");
	//      if (!compute()) {
	//              return;
	//      }
	//      this->value = res;

	// in linux, I need to reinstate the preamble when rendering. No idea why.
	input.preamble =
	        QString("\\usepackage{amssymb,mathtools,mathrsfs}"); // add functions here \n\\DeclareMathOperator\\cis{cis}
//	        QString("\\usepackage{amssymb,amsmath,mathrsfs}"); // add functions here \n\\DeclareMathOperator\\cis{cis}

	input.mathmode = "";
	input.bypassTemplate = true;

//	input.latex = "\\documentclass{article}"
//	              "\\usepackage{amsmath}"
//	              "\\begin{document}"
//	              "\\begin{align*} {{x}^ {0}} &= {1}\\\\ &= 1\\end{align*}"
//	              "\\end{document}";


	input.latex = "\\documentclass{article}"
	              "\\usepackage{amssymb,amsmath,mathtools,mathrsfs}"
	              "\\usepackage[dvipsnames]{xcolor}"
	              "\\definecolor{fg}{HTML}{" + _color.name().mid(1) + "}"
	              "\\definecolor{bg}{HTML}{" + _bg_color.name().mid(1) + "}"
	              "\\begin{document}"
	              "\\color{fg}"
	              "\\pagecolor{bg}"
	              "\\begin{align*}" +
	              Muf::toLatex(ui->eqnInput_adv->toPlainText()) +
	              "\\\\" + "" + " &= " + roundValue + "" +
	              "\\end{align*}"
	              "\\pagenumbering{gobble}"
	              "\\end{document}";
//	input.latex = "\\documentclass{article}"
//	              "\\usepackage{tikz}"
//	              "\\begin{document}"
//	              "\\textcolor{white}{.}\\\\"
//	              "  \\begin{tikzpicture}[domain=0:4]"
//	              "    \\draw[very thin,color=gray] (-0.1,-1.1) grid (3.9,3.9);"
//	              "    \\draw[->] (-0.2,0) -- (4.2,0) node[right] {$x$};"
//	              "    \\draw[->] (0,-1.2) -- (0,4.2) node[above] {$f(x)$};"
//	              "    \\draw[color=red]    plot (\\x,\\x)             node[right] {$f(x) =x$};"
//	              "    \\draw[color=blue]   plot (\\x,{sin(\\x r)})    node[right] {$f(x) = \\sin x$};"
//	              "    \\draw[color=orange] plot (\\x,{0.05*exp(\\x)}) node[right] {$f(x) = \\frac{1}{20} \\mathrm e^x$};"
//	              "  \\end{tikzpicture}"
//	              "\\textcolor{white}{.}\\\\\\textcolor{white}{.}"
//	              "\\pagenumbering{gobble}"
//	              "\\end{document}";
//	input.latex = "\\begin{align*}" +
//	              Muf::toLatex(ui->eqnInput_adv->toPlainText()) +
//	              "\\\\" + "" + " &= " + roundValue + "" +
//	              "\\end{align*}";
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
StrahCalc::updatePreviewBuilderThreadInput_plot()
{
	//      if (!compute()) {
	//              return;
	//      }
	//      this->value = res;

	// in linux, I need to reinstate the preamble when rendering. No idea why.
	input.preamble =
	        QString("\\usepackage{amssymb,mathtools,mathrsfs}"); // add functions here \n\\DeclareMathOperator\\cis{cis}
//	        QString("\\usepackage{amssymb,amsmath}"); // add functions here \n\\DeclareMathOperator\\cis{cis}

	input.bypassTemplate = true;

//	QString s = ui->eqnInput_plot->toPlainText();
//	s.replace("\\x", "x");
//	Muf::toLatex.mPar(s);
	input.latex = "\\documentclass{article}"
	              "\\usepackage{tikz}"
	              "\\begin{document}"
	              "\\textcolor{white}{.}\\\\"
	              "\\begin{tikzpicture}[domain=0.00000001:4]"
	              "\\draw[very thin,color=gray] (-0.1,-1.1) grid (3.9,3.9);"
	              "\\draw[->] (-0.2,0) -- (4.2,0) node[right] {$x$};"
	              "\\draw[->] (0,-1.2) -- (0,4.2) node[above] {$y$};";
	char c = 'f';
	for (QString el : ui->eqnInput_plot->toPlainText().split("\n")) {
//		QString s = el;
		Muf::toLatex.mPar(el);
		el.replace("x", "\\x");
		input.latex += "\\draw[color=black] plot (\\x," // domain here
//		               + ui->eqnInput_plot->toPlainText() +
		               + el +
		               ") node[right] {$" + c + "(x) = "
		               + Muf::toLatex.mPar.tree->toLatex() +
		               "$};";
		if (c == 'z') {
			c = 'f';
		} else {
			++c;
		}
	}
	input.latex += "\\end{tikzpicture}"
	               "\\textcolor{white}{.}\\\\\\textcolor{white}{.}"
	               "\\pagenumbering{gobble}"
	               "\\end{document}";

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
StrahCalc::updateExprtkInput(const QString& input)
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
StrahCalc::compute()
{
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview_adv);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview_plot);

	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput);
	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput_adv);
	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput_plot);

	// display updated expression image
	connect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	        this, &StrahCalc::showRealTimePreview,
	        Qt::QueuedConnection);
	// start rendering result
	connect(this, &StrahCalc::resultAvailable,
	        this, &StrahCalc::updatePreviewBuilderThreadInput,
	        Qt::QueuedConnection);
	updateExprtkInput(ui->eqnInput->text());
	updateHistory(ui->eqnInput->text());
}

void
StrahCalc::compute_adv()
{
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview_adv);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview_plot);

	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput);
	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput_adv);
	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput_plot);

	// display updated expression image
	connect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	        this, &StrahCalc::showRealTimePreview_adv,
	        Qt::QueuedConnection);
	// start rendering result
	connect(this, &StrahCalc::resultAvailable,
	        this, &StrahCalc::updatePreviewBuilderThreadInput_adv,
	        Qt::QueuedConnection);
	updateExprtkInput(ui->eqnInput_adv->toPlainText());
	updateHistory(ui->eqnInput_adv->toPlainText());
}

void
StrahCalc::compute_plot()
{
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview_adv);
	disconnect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	           this, &StrahCalc::showRealTimePreview_plot);

	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput);
	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput_adv);
	disconnect(this, &StrahCalc::resultAvailable,
	           this, &StrahCalc::updatePreviewBuilderThreadInput_plot);

	// display graph
	connect(mPreviewBuilderThread, &KLFPreviewBuilderThread::previewAvailable,
	        this, &StrahCalc::showRealTimePreview_plot,
	        Qt::QueuedConnection);
	updatePreviewBuilderThreadInput_plot();
}

void
StrahCalc::updateHistory(const QString& input)
{
	if (input.isEmpty()) {
		return;
	}
	ui->history_ls->addItem(input);
	ui->history_ls->scrollToBottom();
}

void
StrahCalc::showRealTimePreview(const QImage& preview, bool latexerror)
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
StrahCalc::showRealTimePreview_adv(const QImage& preview, bool latexerror)
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

void
StrahCalc::showRealTimePreview_plot(const QImage& preview, bool latexerror)
{
	if (latexerror) {
		//              ui->statusBar->showMessage(Muf::translation("render_err_general"));
		setStatusMessage("render_err_general");
	} else {
		//              ui->statusBar->showMessage(Muf::translation("done"));
		setStatusMessage("done");
		pixmap = QPixmap::fromImage(preview);
		ui->label_plot->setPixmap(pixmap);
		ui->label_plot->adjustSize();
	}
}
