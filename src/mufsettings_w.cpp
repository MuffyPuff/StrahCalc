#include "mufsettings_w.h"
#include "ui_mufsettings_w.h"

MufSettings_w::MufSettings_w(QDialog* parent) :
        QDialog(parent),
        ui(new Ui::MufSettings_w)
{
	ui->setupUi(this);

	languages       = ui->lang_v;
	timeout         = ui->timeout_v;
	dpi             = ui->dpi_v;
	color           = ui->color_v;
	bg_color        = ui->bg_color_v;
	show_plot       = ui->plot_v;
	reduce          = ui->reduce_v;

	updateText();

	connect(ui->defaults_b, &QPushButton::pressed,
	[ = ]() {
		emit defaults();
	});
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
	ui->lang_l->setText(Muf::translation("language"));
	ui->timeout_l->setText(Muf::translation("timeout"));
	ui->dpi_l->setText(Muf::translation("dpi"));
	ui->color_l->setText(Muf::translation("color"));
	ui->bg_color_l->setText(Muf::translation("bg_color"));
	ui->defaults_b->setText(Muf::translation("defaults"));
	ui->plot_l->setText(Muf::translation("calc_plot"));
	ui->plot_v->setText(Muf::translation("enable"));
	ui->reduce_l->setText(Muf::translation("reduce"));
	ui->reduce_v->setText(Muf::translation("enable"));
	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(
	        Muf::translation("cancel"));
}
