#include "variablelistview.h"

VariableListView::VariableListView()
{

}

bool
VariableListView::addItem(const QString &name, const double &value)
{
	return addItem({new QStandardItem(name), new QStandardItem(value)});
}

bool
VariableListView::addItem(const QString &name, const QString &value)
{
	return addItem({new QStandardItem(name), new QStandardItem(value)});
}

bool
VariableListView::removeItem(const QString &name)
{
	return removeItem(mModel->findItems(name).at(0)->row());
}

bool
VariableListView::editItem(const QString &name, const double &value)
{
	return editItem(mModel->findItems(name).at(0)->row(), 1,
	                new QStandardItem(value));
}

bool
VariableListView::editItem(const QString &name, const QString &value)
{
	return editItem(mModel->findItems(name).at(0)->row(), 1,
	                new QStandardItem(value));
}

bool
VariableListView::moveItem(const QString &name, AbstractList::Direction d)
{
	return moveItem(mModel->findItems(name).at(0)->row(), d);
}
