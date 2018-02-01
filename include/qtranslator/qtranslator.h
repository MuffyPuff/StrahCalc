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
	explicit QTranslator(const QString& lang, QObject* parent = 0);
	~QTranslator();

	QString         operator()(const QString& code, const QString& lang = "");
	static bool     changeLanguage(const QString& lang);

private:
	static bool     loadLangFile(const QString& lang = "");

signals:

public slots:

private:
	static QString  _language;
	static QJsonArray _languageFile;

	static QMutex   _mutex;
	static QWaitCondition _condnewinfoavail;

	static bool     _hasnewinfo;
	static bool     _abort;
	static QString  languageDir;
};

#endif // QTRANSLATOR_H
