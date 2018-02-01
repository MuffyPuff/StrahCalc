#include "qtranslator.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QString QTranslator::_language = "en-GB";
QJsonArray QTranslator::_languageFile = QJsonArray();

QMutex QTranslator::_mutex;
QWaitCondition QTranslator::_condnewinfoavail;

bool QTranslator::_hasnewinfo = false;
bool QTranslator::_abort = false;

QString QTranslator::languageDir = QDir::homePath() +
                                   "/.strah_calc/lang/en-GB.json";

QTranslator::QTranslator(const QString &lang, QObject *parent)
        : QObject(parent)
//        : _language(lang), QObject(parent), _hasnewinfo(false), _abort(false)
{
	languageDir = QDir::homePath() +
	              "/.strah_calc/lang/";
	changeLanguage(lang);
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
//	qDebug() << "loading file";
	QMutexLocker mutexlocker(&_mutex);
//	qDebug() << "locked mutex";
	QString language = _language;
	if (!lang.isEmpty()  and lang.compare(language) != 0) {
		language = lang;
	}
	QFile lingo(languageDir + language + ".json");
//	qDebug() << "set file";
//	QDir dir(languageDir);
//	qDebug() << dir.absolutePath();
//	qDebug() << languageDir;
//	qDebug() << dir.homePath();
	if (!lingo.open(QIODevice::ReadOnly)) {
		qDebug() << "language file not found";
		return false;
	}
//	qDebug() << "language file found";
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
//	QMutexLocker mutexlocker(&_mutex);
//	qDebug() << "changing language";
	_mutex.lock();
	if (_language == lang) {
		_mutex.unlock();
		return false;
	}
	_language = lang;
	_hasnewinfo = true;
	_mutex.unlock();
//	qDebug() << "calling loadLangFile";
	loadLangFile();
//	qDebug() << "called loadLangFile";
	_mutex.lock();
	_condnewinfoavail.wakeOne();
	_mutex.unlock();
	return true;
}

