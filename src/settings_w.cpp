#include "settings_w.h"
#include "ui_settings_w.h"

settings_w::settings_w(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::settings_w)
{
	ui->setupUi(this);
}

settings_w::~settings_w()
{
	delete ui;
}
