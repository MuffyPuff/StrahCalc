#include "mufsettings_w.h"
#include "ui_settings_w.h"

MufSettings_w::MufSettings_w(QDialog* parent) :
        QDialog(parent),
        ui(new Ui::MufSettings_w)
{
	ui->setupUi(this);
}

MufSettings_w::~MufSettings_w()
{
	delete ui;
}
