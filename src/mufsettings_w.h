#ifndef SETTINGS_W_H
#define SETTINGS_W_H

#include <QWidget>

namespace Ui {
class settings_w;
}

class settings_w : public QWidget
{
	Q_OBJECT

public:
	explicit settings_w(QWidget *parent = 0);
	~settings_w();

private:
	Ui::settings_w *ui;
};

#endif // SETTINGS_W_H
