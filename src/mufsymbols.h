#ifndef MUFSYMBOLS_H
#define MUFSYMBOLS_H

#include <QObject>
#include <QList>
#include <QRegularExpression>
#include <QStack>
#include <QQueue>

class MufSymbols : public QObject
{
	Q_OBJECT

public:
	enum class Assoc {
		NONE,
		LEFT,
		RIGHT
	};
	Q_ENUM(Assoc)
	struct str_tok_t {
		QString         s;
		Assoc           assoc;
		int             len;
		int             prec;
	};
	struct pat_t {
		QString         str;
		Assoc           assoc;
		int             prec;
		QRegularExpression re;
	};
private:

public:
	explicit MufSymbols(QObject* parent = nullptr);

private:

	int             init();
	pat_t*          match(
	        QString s,
	        QList<pat_t> p,
	        str_tok_t* t,
	        QString* e);
	void            display(QString s);

public:
	int             parse(QString s);

signals:

public slots:

private:
	pat_t           pat_eos;
	QList<pat_t>    pat_ops;
	QList<pat_t>    pat_arg;

	QStack<str_tok_t> stack;

public:
	QQueue<str_tok_t> queue;
};

#endif // MUFSYMBOLS_H
