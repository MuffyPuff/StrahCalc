#ifndef MUFSETTINGS_W_H
#define MUFSETTINGS_W_H

#include <QDialog>

#include <QComboBox>
#include <QSpinBox>

#include "global.h"

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

	QComboBox*      languages;
	QSpinBox*       timeout;

public slots:
	void            updateText(const QString& lang = "");

private:
	Ui::MufSettings_w* ui;
};

#endif // MUFSETTINGS_W_H
