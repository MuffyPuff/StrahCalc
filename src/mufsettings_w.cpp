#include "mufsettings_w.h"
#include "ui_settings_w.h"

settings_w::settings_w(QDialog* parent) :
        QDialog(parent),
        ui(new Ui::settings_w)
{
	ui->setupUi(this);
}

settings_w::~settings_w()
{
	delete ui;
}
