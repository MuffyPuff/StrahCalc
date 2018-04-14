#ifndef MUFLATEX_H
#define MUFLATEX_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>

#include "mufexprparser.h"

class MufLatex : public QObject
{
	Q_OBJECT
public:
	explicit MufLatex(QObject* parent = nullptr);

	QString         operator()(QString input, const bool& reduce = true);

private:
	QString         assignment(QString input);
	QString         power(QString input);
	QString         operators(QString input);
	QString         braces(QString input);

signals:

public slots:

private:

	// for future use
//	static QMutex   _mutex;
//	static QWaitCondition _condnewinfoavail;

//	static bool     _hasnewinfo;
//	static bool     _abort;

public:
	MufExprParser   mPar;
};

#endif // MUFLATEX_H
