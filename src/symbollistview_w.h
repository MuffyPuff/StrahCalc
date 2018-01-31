#ifndef SYMBOLLISTVIEW_W_H
#define SYMBOLLISTVIEW_W_H

#include <QWidget>
#include <QStandardItemModel>

#include "global.h"

namespace Ui
{
class SymbolListView_w;
}

class SymbolListView_w : public QWidget
{
	Q_OBJECT

public:
	explicit SymbolListView_w(QStringList header, QWidget *parent = 0);
	~SymbolListView_w();

	enum class Direction {UP, DOWN, LEFT, RIGHT};
	Q_ENUM(Direction)

//	bool setHeaderText(const int &index, const Qstring &text);
//	QStringList header;
protected:

public slots:
	bool addItem();
	bool removeItem();

signals:

	void addSym(const QString &, const double &);
	void remSym(const QString &);

protected:
	bool addItem(QList<QStandardItem *> item);
	bool addItem(const int &row, QList<QStandardItem *> item);
	bool removeItem(const int &row);
	bool editItem(const int &row, QList<QStandardItem *> item);
	bool editItem(const int &row, const int &col, QStandardItem *item);
	bool moveItem(const int &row, Direction d);

protected:
	Ui::SymbolListView_w *ui;
	QStandardItemModel *mModel;

public:
};

#endif // SYMBOLLISTVIEW_W_H
