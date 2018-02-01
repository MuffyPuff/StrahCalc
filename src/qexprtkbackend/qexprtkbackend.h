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
template <typename T>
class symbol_table;
template <typename T>
class expression;
template <typename T>
class parser;

namespace parser_error
{
struct type;
}
}
//#include "gmpxx.h"

class QExprtkBackend : public QThread
{
	Q_OBJECT

	typedef exprtk::symbol_table<double> symbol_table_t;
	typedef exprtk::expression<double>     expression_t;
	typedef exprtk::parser<double>             parser_t;
	typedef QPair<std::string, double *>       symbol_t;
	typedef exprtk::parser_error::type      parse_err_t;

public:
	explicit QExprtkBackend(QObject *parent, const QString &in);
	virtual ~QExprtkBackend();

	double          output;
	QList<parse_err_t> error_list;

	bool addVariable(QString name, const double value);
	bool addConstant(QString name, const double value);

	void            run();

signals:
	void            resultAvailable(double);
	void            error();

public slots:
	bool            inputChanged(const QString &in);

protected:
	QList<symbol_t> _variables;
	QList<symbol_t> _constants;
	QString         _input;

	QMutex          _mutex;
	QWaitCondition  _condnewinfoavail;

	bool            _hasnewinfo;
	bool            _abort;

};

#endif // QEXPRTKBACKEND_H
