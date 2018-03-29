#include "mufsettings_w.h"
#include "ui_mufsettings_w.h"

MufSettings_w::MufSettings_w(QDialog* parent) :
        QDialog(parent),
        ui(new Ui::MufSettings_w)
{
	ui->setupUi(this);

	languages =     ui->lang_v;
	timeout =       ui->timeout_v;
	dpi =           ui->dpi_v;

	updateText();
}

MufSettings_w::~MufSettings_w()
{
	delete ui;
}

void
MufSettings_w::updateText(const QString& lang)
{
	Q_UNUSED(lang);

	ui->tabWidget->setTabText(0, Muf::translation("general"));
	ui->lang_l->setText(Muf::translation("languages"));
	ui->timeout_l->setText(Muf::translation("timeout"));
	ui->dpi_l->setText(Muf::translation("dpi"));
}


