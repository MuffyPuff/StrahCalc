#ifndef QEXPRTKBACKEND_H
#define QEXPRTKBACKEND_H

#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QString>
#include <QList>
#include <QPair>
#include <QMutex>
#include <QMutexLocker>

//#include "exprtk.hpp"
namespace exprtk
{
namespace parser_error
{
struct type;
}
}
//#include "gmpxx.h"

class QExprtkBackend : public QThread
{
	Q_OBJECT
public:
	explicit QExprtkBackend(QObject *parent, QString in);
	virtual ~QExprtkBackend();

	double output;
	QList<exprtk::parser_error::type> error_list;

	bool addVariable(QString name, double value);
	bool addConstant(QString name, double value);

	void run();

signals:
	void resultAvailable(double);
	void error();

public slots:
	bool inputChanged(const QString &in);

protected:
	QList<QPair<std::string, double *>> _variables;
	QList<QPair<std::string, double *>> _constants;
	QString _input;

	QMutex _mutex;
	QWaitCondition _condnewinfoavail;

	bool _hasnewinfo;
	bool _abort;

};

#endif // QEXPRTKBACKEND_H
