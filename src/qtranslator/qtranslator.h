#ifndef QTRANSLATOR_H
#define QTRANSLATOR_H

#include <QObject>
//#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>

#include <QStandardPaths>

class QTranslator : public QObject
{
	Q_OBJECT
public:
	explicit QTranslator(const QString &lang, QObject *parent = 0);
	~QTranslator();

	QString operator()(const QString &code, const QString &lang = "");
	bool changeLanguage(const QString &lang);

private:
	bool loadLangFile(const QString &lang = "");

signals:

public slots:

private:
	QString _language;
	QJsonArray _languageFile;

	QMutex _mutex;
	QWaitCondition _condnewinfoavail;

	bool _hasnewinfo;
	bool _abort;
	QString languageDir;
};

#endif // QTRANSLATOR_H
