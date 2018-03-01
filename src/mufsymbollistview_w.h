#ifndef MUFSYMBOLLISTVIEW_W_H
#define MUFSYMBOLLISTVIEW_W_H

#include <QWidget>
#include <QStandardItemModel>
#include <QList>
#include <QPair>
#include <string>

#include "global.h"
// TODO: REMOVE!!!!!!
#include "mufexprtkbackend.h" // TEMP!!!!

namespace Ui
{
class MufSymbolListView_w;
}

class MufSymbolListView_w : public QWidget
{
	Q_OBJECT

public:
	explicit MufSymbolListView_w(QStringList header, QWidget* parent = 0);
	~MufSymbolListView_w();

	enum class Direction {UP, DOWN, LEFT, RIGHT};
	Q_ENUM(Direction)

	bool            setList(const QList<MufExprtkBackend::symbol_t<double>>& list);
//	bool            setHeaderText(const int &index, const Qstring &text);
//	QStringList     header;

public slots:
	bool            addItem();
	bool            removeItem();
	void            updateText(const QString& lang = "");

signals:
	void            addSym(const QString&, const double&);
	void            remSym(const QString&);

protected:
	bool            addItem(QList<QStandardItem*> item);
	bool            addItem(const int& row, QList<QStandardItem*> item);
	bool            removeItem(const int& row);
	bool            editItem(const int& row, QList<QStandardItem*> item);
	bool            editItem(const int& row, const int& col, QStandardItem* item);
	bool            moveItem(const int& row, Direction d);


protected:
	Ui::MufSymbolListView_w* ui;
	QStandardItemModel* mModel;

	QStringList     _header;
};

#endif // MUFSYMBOLLISTVIEW_W_H
