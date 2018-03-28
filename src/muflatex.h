#ifndef MUFLATEX_H
#define MUFLATEX_H

#include <QObject>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>

class MufLatex : public QObject
{
	Q_OBJECT
public:
	explicit MufLatex(QObject* parent = nullptr);

	QString         operator()(QString input);

private:

signals:

public slots:

private:

	// for future use
//	static QMutex   _mutex;
//	static QWaitCondition _condnewinfoavail;

//	static bool     _hasnewinfo;
//	static bool     _abort;
};

#endif // MUFLATEX_H
