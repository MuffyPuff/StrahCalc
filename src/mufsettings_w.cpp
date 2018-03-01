#include "mufsettings_w.h"
#include "ui_mufsettings_w.h"

MufSettings_w::MufSettings_w(QDialog* parent) :
        QDialog(parent),
        ui(new Ui::MufSettings_w)
{
	ui->setupUi(this);

	languages = ui->lang_cb;
}

MufSettings_w::~MufSettings_w()
{
	delete ui;
}
