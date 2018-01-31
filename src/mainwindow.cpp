#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "klfbackend.h"
//#include "exprtk.hpp"
#include <QDebug>
#include <cmath>

//using global::translation;


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)//,
//        translation("en-GB", this)
{
	ui->setupUi(this);

	input.mathmode = "\\[ ... \\]";
//	input.bypassTemplate = true;

	input.dpi = 150;
	ui->label->setMinimumHeight(input.dpi / 2);

//	input.bg_color = qRgba(225, 225, 225, 225);
	input.preamble =
	        QString("\\usepackage{amssymb,amsmath,mathrsfs}");

	if (!KLFBackend::detectSettings(&settings, "./texlive")) {
		qDebug() << "unable to find LaTeX in default directories.";
	} else {
		qDebug() << "default settings working!";
	}

	// setup variables:
	mPreviewBuilderThread = new KLFPreviewBuilderThread(this, input, settings);
	mExprtk = new QExprtkBackend(this, input.latex);

	qDebug() << translation("language_code");

	// make connections
	connect(ui->plainTextEdit, SIGNAL(textChanged()),
	        this, SLOT(updateExprtkInput()),
	        Qt::QueuedConnection);
	connect(mExprtk, SIGNAL(resultAvailable(double)), // TODO: add var update
	        this, SLOT(getResult(double)),
	        Qt::QueuedConnection);
	connect(this, SIGNAL(resultAvailable()),
	        this, SLOT(updatePreviewBuilderThreadInput()),
	        Qt::QueuedConnection);
	connect(mExprtk, SIGNAL(error()),
	        this, SLOT(handleExprtkError()),
	        Qt::QueuedConnection);
	connect(mPreviewBuilderThread, SIGNAL(previewAvailable(const QImage &, bool)),
	        this, SLOT(showRealTimePreview(const QImage &, bool)),
	        Qt::QueuedConnection);
	connect(ui->clipBtnEq, SIGNAL(clicked()),
	        this, SLOT(copyEqToClipboard()));
	connect(ui->clipBtnRes, SIGNAL(clicked()),
	        this, SLOT(copyResToClipboard()));
//	connect(ui->addVar_b, SIGNAL(clicked()),
//	        this, SLOT(addNewVariable()));
//	connect(ui->, SIGNAL(clicked()),
//	        this, SLOT());

	QMenu *menu = ui->menuBar->addMenu(translation("tools"));
//	QToolBar *toolBar = addToolBar(translation("tools"));
	QAction *act = new QAction(translation("options"), this);
	menu->addAction(act);
//	toolBar->addAction(act);

	// rename tabs
	ui->tabWidget->setTabText(0, translation("calc"));
//	ui->tabWidget->setTabText(1, translation("vars"));
	ui->tabWidget->setTabText(2, translation("vars"));
	ui->tabWidget->setTabText(3, translation("consts"));

//	ui->varName_l->setText(translation("var_name"));
//	ui->varValue_l->setText(translation("var_value"));
//	ui->addVar_b->setText(translation("add_var"));

//	/* model init */
//	model = new QStandardItemModel(this);
////    model->setItem(0, 0, new QStandardItem("name"));
////    model->setItem(0, 1, new QStandardItem("value"));
//	model->appendRow({new QStandardItem("name"), new QStandardItem("value")});
//	qDebug() << model->setHeaderData(0, Qt::Horizontal, translation("name"));
//	qDebug() << model->setHeaderData(1, Qt::Horizontal, translation("value"));
//	model->removeRow(0);
//	ui->varList->setModel(model);
//	ui->varList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//	ui->varList->verticalHeader()->setVisible(false);

//	ui->tab_2->setVisible(false);
//	ui->tab_2->hide();
	ui->tabWidget->removeTab(1); // comment out tab i don't need right now

	// sybol view init
	mVarList   = new SymbolListView_w({translation("name"), translation("value")},
	                                  this);
	mConstList = new SymbolListView_w({translation("name"), translation("value")},
	                                  this);

	ui->tab_3->layout()->addWidget(mVarList);
	ui->tab_4->layout()->addWidget(mConstList);

	// add symbols from view to exprtk
	connect(mVarList, SIGNAL(addSym(const QString &, const double &)),
	        this, SLOT(addVariable(const QString &, const double &)));
	connect(mVarList, SIGNAL(remSym(const QString &)),
	        this, SLOT(removeVariable(const QString &)));
	connect(mConstList, SIGNAL(addSym(const QString &, const double &)),
	        this, SLOT(addConstant(const QString &, const double &)));
	connect(mConstList, SIGNAL(remSym(const QString &)),
	        this, SLOT(removeConstant(const QString &)));


	ui->plainTextEdit->setFocus();
//	ui->statusBar->showMessage("Waiting for input...");
	ui->statusBar->showMessage(translation("wait"));
}

//QString
//MainWindow::roundFloat(double value, int count)
//{
//	//      qDebug() << "round " << double(value);
//	if (count > 15) {
////            return QString::number(value * pow(10, -count), 'g', prec);
//		return QString::number(value, 'g', prec);
//	}
//	int whole = round(value);
//	double eps = 1e-12;
//	if (value > whole - eps and value < whole + eps) {
////            return QString::number(whole * pow(10, -count), 'g', prec);
//		return QString::number(whole, 'g', prec);
//	} else {
//		return QString::number(roundFloat(value * 10, count + 1).toDouble() * 0.1,
//		                       'g', prec);
//		//this->roundValue = QString::number(this->roundValue.toDouble() * 0.1);
//	}
//}

void
MainWindow::getResult(double value)
{
	this->rawValue = value;
	this->roundValue = roundFloat(value);
	emit resultAvailable();
////    qDebug() << "round " << double(value);
//	if (count > 15) {
//		this->roundValue = QString::number(value * pow(10, -count), 'g', prec);
//		emit resultAvailable();
//		return;
//	}
//	int whole = round(value);
//	double eps = 1e-12;
////    qDebug() << "Value: " << QString::number(value, 'e', 25)
////             << " Whole: " << whole
////             << " eps: " << QString::number(eps, 'e', 25);
//	if (value > whole - eps and value < whole + eps) {
//		this->roundValue = QString::number(whole * pow(10, -count), 'g', prec);
//		emit resultAvailable();
//		return;
//	} else {
//		getResult(value * 10, count + 1);
//		//this->roundValue = QString::number(this->roundValue.toDouble() * 0.1);
	//      }
}

void
MainWindow::addVariable(const QString &name, const double &value)
{
	mExprtk->addVariable(name, value);
}

void
MainWindow::removeVariable(const QString &name)
{
	// TODO: implement removal
	Q_UNUSED(name);
}

void
MainWindow::addConstant(const QString &name, const double &value)
{
	mExprtk->addConstant(name, value);
}

void
MainWindow::removeConstant(const QString &name)
{
	// TODO: implement removal
	Q_UNUSED(name);
}
//template <typename T>
//inline T equal_impl(const T v0, const T v1, real_type_tag)
//{
//   const T epsilon = epsilon_type<T>::value();
//   return (abs_impl(v0 - v1,real_type_tag()) <= (std::max(T(1),std::max(abs_impl(v0,real_type_tag()),abs_impl(v1,real_type_tag()))) * epsilon)) ? T(1) : T(0);
//}

void
MainWindow::copyEqToClipboard()
{
	clipboard->setPixmap(pixmap);
	ui->statusBar->showMessage(translation("img_copied"));
}

void
MainWindow::copyResToClipboard()
{
//	clipboard->setText(QString::number(this->value, 'e', acc));
	clipboard->setText(roundValue);
	ui->statusBar->showMessage(translation("res_copied"));
}

//void
//MainWindow::addNewVariable()
//{
//	QString name  = ui->varName_i->text();
//	double value = ui->varValue_i->text().toDouble();
//	ui->varName_i->clear();
//	ui->varValue_i->clear();
////    model->appendRow({new QStandardItem(name), new QStandardItem(QString::number(value, 'g', prec))});
//	model->appendRow({new QStandardItem(name), new QStandardItem(roundFloat(value))});
//	mExprtk->addVariable(name, value);
//}


MainWindow::~MainWindow()
{
	delete mPreviewBuilderThread;
	delete mExprtk;
	delete ui;
}

//bool
//MainWindow::compute()
//{
//	typedef exprtk::symbol_table<double> symbol_table_t;
//	typedef exprtk::expression<double>     expression_t;
//	typedef exprtk::parser<double>             parser_t;

//	QString expr_input = ui->plainTextEdit->toPlainText();

//	symbol_table_t symbol_table;
//	symbol_table.add_constants();

//	expression_t expression;
//	expression.register_symbol_table(symbol_table);

//	parser_t parser;

//	if (!parser.compile(expr_input.toStdString(), expression)) {
//		qDebug() << "expression compilation error...";
//		ui->statusBar->showMessage("Not a valid expression");
//		return false;
//	}

//	value = expression.value();
//	return true;
//}

void
MainWindow::handleExprtkError()
{
	// TODO: halt render
//	QList<exprtk::parser_error::type> errors = mExprtk->error_list;
	ui->statusBar->showMessage(translation("calc_err_general"));
}

void
//MainWindow::updatePreviewBuilderThreadInput(const double &res)
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

	input.latex = ui->plainTextEdit->toPlainText() +
	              " = " +
	              roundValue;
//	              QString::number(this->value, 'e', acc);
	if (mPreviewBuilderThread->inputChanged(input)) {
		qDebug() << "input changed. Render...";
		ui->statusBar->showMessage(translation("rendering"));
		mPreviewBuilderThread->start();
	} else {
		ui->statusBar->showMessage(translation("done"));
	}
}

void
MainWindow::updateExprtkInput()
{
	QString input(ui->plainTextEdit->toPlainText());
	if (mExprtk->inputChanged(input)) {
		qDebug() << "input changed. Calculate...";
		ui->statusBar->showMessage(translation("calculating"));
		mExprtk->start();
	}
}

void
MainWindow::showRealTimePreview(const QImage &preview, bool latexerror)
{
	if (latexerror) {
		ui->statusBar->showMessage(translation("render_err_general"));
//		ui->statusBar->showMessage("Unable to render your equation. Please double check.");
	} else {
		ui->statusBar->showMessage(translation("done"));
		pixmap = QPixmap::fromImage(preview);
		ui->label->setPixmap(pixmap);
		ui->label->adjustSize();
	}
}

