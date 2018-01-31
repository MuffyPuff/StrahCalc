#ifndef VARIABLELISTVIEW_H
#define VARIABLELISTVIEW_H

#include <QWidget>
#include "abstractlist.h"

class VariableListView : public AbstractList
{
public:
	VariableListView();
	bool addItem(const QString &name, const double &value);
	bool addItem(const QString &name, const QString &value);
	bool removeItem(const QString &name);
	bool editItem(const QString &name, const double &value);
	bool editItem(const QString &name, const QString &value);
	bool moveItem(const QString &name, Direction d);

};

#endif // VARIABLELISTVIEW_H
