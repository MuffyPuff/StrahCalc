#include "symbollistview_w.h"
#include "ui_symbollistview_w.h"

#include <QDebug>

/**
 * TODO: mutex
 * TODO: add dynamic columns
 */


SymbolListView_w::SymbolListView_w(QStringList header, QWidget* parent) :
        QWidget(parent),
        ui(new Ui::SymbolListView_w)
{
	ui->setupUi(this);

	mModel = new QStandardItemModel(this);
	ui->list->setModel(mModel);
	ui->list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->list->verticalHeader()->setVisible(false);

	// connect ui elements to slots
	connect(ui->add_b, SIGNAL(clicked()),
	        this, SLOT(addItem()));
	connect(ui->remove_b, SIGNAL(clicked()),
	        this, SLOT(removeItem()));

	// rename list header
	for (int i = 0; i < header.size(); ++i) {
		mModel->appendColumn({new QStandardItem("tmp")});
		mModel->setHeaderData(i, Qt::Horizontal, header.at(i));
	}
	mModel->removeRow(0);

	// rename labels and buttons
	ui->add_b->setText(Muf::translation("add"));
	ui->remove_b->setText(Muf::translation("remove"));
	ui->name_l->setText(Muf::translation("name"));
	ui->value_l->setText(Muf::translation("value"));
}

SymbolListView_w::~SymbolListView_w()
{
	for (int row = 0; row < mModel->rowCount(); ++row) {
		foreach (QStandardItem* i, mModel->takeRow(row)) {
			delete i;
		}
	}
	mModel->clear();
	delete ui;
}

bool
SymbolListView_w::addItem()
{
	QString name  = ui->name_i->text();
	QString value = ui->value_i->text();
	ui->name_i->clear();
	ui->value_i->clear();
	emit addSym(name, value.toDouble());
	return addItem({new QStandardItem(name), new QStandardItem(value)});
}

bool
SymbolListView_w::addItem(QList<QStandardItem*> item)
{
	mModel->appendRow(item);
	return true;
}

bool
SymbolListView_w::addItem(const int& row, QList<QStandardItem*> item)
{
	mModel->insertRow(row, item);
	return true;
}

bool
SymbolListView_w::removeItem()
{
	QModelIndex index = ui->list->selectionModel()->currentIndex();
	if (index.data().toString() == "") {
		qDebug() << "No item selected";
		return false;
	}
	emit remSym(mModel->takeItem(index.row())->text());
	return removeItem(index.row());
}

bool
SymbolListView_w::removeItem(const int& row)
{
	foreach (QStandardItem* i, mModel->takeRow(row)) {
		delete i;
	}
	mModel->removeRow(row);
	return true;
}

bool
SymbolListView_w::editItem(const int& row, QList<QStandardItem*> item)
{
	for (int i = 0; i < item.size(); ++i) {
		mModel->setItem(row, i, item.at(i));
	}
	return true;
}

bool
SymbolListView_w::editItem(const int& row, const int& col, QStandardItem* item)
{
	mModel->setItem(row, col, item);
	return true;
}

bool
SymbolListView_w::moveItem(const int& row, SymbolListView_w::Direction d)
{
	// TODO: implement moveItem
	Q_UNUSED(row)
	Q_UNUSED(d)
	return false;
}

bool
SymbolListView_w::setList(const QList<MufExprtkBackend::symbol_t<double>>& list)
{
//	mModel->clear(); works but clears header
//	qDebug() << list.size();
	mModel->removeRows(0, mModel->rowCount());
	foreach (auto el, list) {
		mModel->appendRow({
		        new QStandardItem(QString::fromStdString(el.name)),
		        new QStandardItem(Muf::roundFloat(*el.value))
		});
	}
	return true;
}
