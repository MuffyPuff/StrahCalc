#ifndef SETTINGS_W_H
#define SETTINGS_W_H

#include <QDialog>

namespace Ui
{
class settings_w;
}

class settings_w : public QDialog
{
	Q_OBJECT

public:
	explicit settings_w(QDialog* parent = 0);
	~settings_w();

private:
	Ui::settings_w* ui;
};

#endif // SETTINGS_W_H
