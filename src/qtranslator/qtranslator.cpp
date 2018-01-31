#include "qtranslator.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QTranslator::QTranslator(const QString &lang, QObject *parent)
        : _language(lang), QObject(parent), _hasnewinfo(false), _abort(false)
{
	languageDir = QDir::homePath() +
	              "/.strah_calc/lang/";
	loadLangFile();
}

QTranslator::~QTranslator()
{
	_mutex.lock();
	_abort = true;
	_condnewinfoavail.wakeOne();
	_mutex.unlock();
}

QString
QTranslator::operator()(const QString &code, const QString &lang)
{
	QJsonArray langFile;
	QString language;
	for (;;) {
		_mutex.lock();
		bool abrt = _abort;
		_mutex.unlock();
		if (abrt) {
			return "aborted";
		}

		_mutex.lock();
		langFile = _languageFile;
		language = _language;
		_hasnewinfo = false;
		_mutex.unlock();

		if (!lang.isEmpty() and lang.compare(language) != 0) {
			qDebug() << "alt language";
//			qDebug() << "lang: " << lang;
//			qDebug() << "language: " << language;
//			qDebug() << (!lang.isEmpty());
//			qDebug() << (lang.compare(language) != 0);
			QFile lingo(languageDir + lang + ".json");
			if (!lingo.open(QIODevice::ReadOnly)) {
				qDebug() << "language file not found";
				return "Language file not found";
			}
			QByteArray langData = lingo.readAll();
			lingo.close();
			QJsonDocument doc = QJsonDocument::fromJson(langData);
			langFile = doc.array();
		}

		_mutex.lock();
		bool abort = _abort;
		bool hasnewinfo = _hasnewinfo;
		_mutex.unlock();

		if (abort) {
			return "aborted";
		}
		if (hasnewinfo) {
			continue;
		}

		QVariantList vl = langFile.toVariantList();
		QVector<QVariant> vv = vl.toVector();
		foreach (QVariant e, vv) {
			QJsonObject entry = e.toJsonObject();
			if (entry.value("id").toString() == code) {
				return entry.value("translation").toString();
			}
		}
		qDebug() << "language entry not found";
		return "Entry not found";
	}
}

bool
QTranslator::loadLangFile(const QString &lang)
{
	QMutexLocker mutexlocker(&_mutex);
	QString language = _language;
	if (!lang.isEmpty()  and lang.compare(language) != 0) {
		language = lang;
	}
	QFile lingo(languageDir + language + ".json");
//	QDir dir(languageDir);
//	qDebug() << dir.absolutePath();
//	qDebug() << languageDir;
//	qDebug() << dir.homePath();
	if (!lingo.open(QIODevice::ReadOnly)) {
		qDebug() << "language file not found";
		return false;
	}
	QByteArray langData = lingo.readAll();
	lingo.close();
	QJsonDocument doc = QJsonDocument::fromJson(langData);
	_languageFile = doc.array();
	_hasnewinfo = true;
	_condnewinfoavail.wakeOne();
	qDebug() << "language changed";
	return true;
}

bool
QTranslator::changeLanguage(const QString &lang)
{
	QMutexLocker mutexlocker(&_mutex);
	if (_language == lang) {
		return false;
	}
	_language = lang;
	loadLangFile();
	_hasnewinfo = true;
	_condnewinfoavail.wakeOne();
	return true;
}

