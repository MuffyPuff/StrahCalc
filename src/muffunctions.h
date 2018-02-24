#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <QObject>

#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>
#include <QList>

#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "mufexprtkbackend.h"


class MufFunctions : public QObject
{
	Q_OBJECT
public:
	explicit MufFunctions(const QStringList& dirList,
	                      MufExprtkBackend* exprtk,
	                      QObject* parent = nullptr);
	~MufFunctions();

private:
	struct muf_function_t {
		muf_function_t() {}

		QString name;
		QString uid;
		QString body;
		QStringList args;
		QFileInfo fileInfo;
	};

private:
	static bool     loadDir(const QDir& dir);
	static bool     loadFile(const QFileInfo& fileInfo,
	                         const QStringList& fileBody);
//	static bool     loadFunction();
	static bool     loadFunctions(const QFileInfo& fileInfo,
	                              const QStringList& fileBody);

signals:

public slots:

private:
	static QMutex   _mutex;
	static QWaitCondition _condnewinfoavail;

	static bool     _hasnewinfo;
	static bool     _abort;

	static QList<QDir> fnDirList;
	static QList<muf_function_t*> fnList;
	static MufExprtkBackend* mExprtk;
};

#endif // FUNCTIONS_H
