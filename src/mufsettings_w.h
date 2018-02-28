#ifndef MUFSETTINGS_W_H
#define MUFSETTINGS_W_H

#include <QDialog>

namespace Ui
{
class MufSettings_w;
}

class MufSettings_w : public QDialog
{
	Q_OBJECT

public:
	explicit MufSettings_w(QDialog* parent = 0);
	~MufSettings_w();

private:
	Ui::MufSettings_w* ui;
};

#endif // MUFSETTINGS_W_H
