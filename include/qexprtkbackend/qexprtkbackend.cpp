#include "qexprtkbackend.h"
#include <QDebug>
#include <QStringLiteral>
#include <cmath>

#include "exprtk.hpp"
//#include "exprtk_mpfr_adaptor.hpp"

QExprtkBackend::QExprtkBackend(QObject* parent, const QString& in)
        : QThread(parent), _input(in), _hasnewinfo(false), _abort(false)
{
}

QExprtkBackend::~QExprtkBackend()
{
//	typedef QPair<std::string, double *>       symbol_t;

	_mutex.lock();
	_abort = true;

	foreach (symbol_t var, _variables) {
		delete var.second;
	}
	foreach (symbol_t con, _constants) {
		delete con.second;
	}

	_condnewinfoavail.wakeOne();
	_mutex.unlock();
	wait();
}

bool
QExprtkBackend::addVariable(const QString& name, const double& value)
{
//	typedef QPair<std::string, double *>       symbol_t;

	QMutexLocker mutexlocker(&_mutex);
	_variables.append(symbol_t(name.toStdString(), new double(value)));
	_hasnewinfo = true;
	_condnewinfoavail.wakeOne();
	return true;
}

bool
QExprtkBackend::addConstant(const QString& name, const double& value)
{
//	typedef QPair<std::string, double *>       symbol_t;

	QMutexLocker mutexlocker(&_mutex);
	_constants.append(symbol_t(name.toStdString(), new double(value)));
	_hasnewinfo = true;
	_condnewinfoavail.wakeOne();
	return true;
}

void
QExprtkBackend::run()
{
	qDebug() << "calculating";
//	typedef exprtk::symbol_table<double> symbol_table_t;
//	typedef exprtk::expression<double>     expression_t;
//	typedef exprtk::parser<double>             parser_t;
//	typedef QPair<std::string, double *>       symbol_t;

	symbol_table_t symbol_table;
	expression_t expression;
	parser_t parser;
	QList<symbol_t> variables;
	QList<symbol_t> constants;
	QString input;
//	parser.enable_unknown_symbol_resolver();

	for (;;) {
		_mutex.lock();
//		qDebug() << "in mutex lock 1";
		bool abrt = _abort;
		_mutex.unlock();
//		qDebug() << "out of mutex lock 1";
		if (abrt) {
//			qDebug() << "abort1";
			return;
		}

		// get vars and input
		_mutex.lock();
		input = _input;
		variables = _variables;
		constants = _constants;
		_hasnewinfo = false;
		_mutex.unlock();

		foreach (symbol_t var, variables) {
			symbol_table.add_variable(var.first, *var.second);
		}

		foreach (symbol_t con, constants) {
			symbol_table.add_constant(con.first, *con.second);
		}

		symbol_table.add_constants();

		expression.register_symbol_table(symbol_table);

		if (!parser.compile(input.toStdString(), expression)) {
			qDebug() << "expression compilation error...";
//			ui->statusBar->showMessage("Not a valid expression");
			for (int i = parser.error_count(); i > 0; --i) {
				error_list.prepend(parser.get_error(i - 1));
			}
			emit error();
			return;
		}

		_mutex.lock();
		bool abort = _abort;
		bool hasnewinfo = _hasnewinfo;
		_mutex.unlock();

		if (abort) {
			return;
		}
		if (hasnewinfo) {
//			qDebug() << "new info?";
			continue;
		}
		output = expression.value();
		emit resultAvailable(output);
//		return;

		_mutex.lock();
		_condnewinfoavail.wait(&_mutex);
		_mutex.unlock();
	}
}

bool
QExprtkBackend::inputChanged(const QString& in)
{
	QMutexLocker mutexlocker(&_mutex);
	if (_input == in) {
		return false;
	}
	_input = in;
	_hasnewinfo = true;
	_condnewinfoavail.wakeOne();
	return true;
}
