#ifndef MUFSETTINGS_W_H
#define MUFSETTINGS_W_H

#include <QDialog>

#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>

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
	QSpinBox*       dpi;
	QLineEdit*      color;
	QLineEdit*      bg_color;
	QCheckBox*      show_plot;
	QCheckBox*      reduce;

signals:
	void            defaults();

public slots:
	void            updateText(const QString& lang = "");

private:
	Ui::MufSettings_w* ui;
};

#endif // MUFSETTINGS_W_H
