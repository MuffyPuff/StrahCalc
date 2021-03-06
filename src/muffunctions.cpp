#include "muffunctions.h"

#include <QDebug>

QMutex MufFunctions::_mutex;
QWaitCondition MufFunctions::_condnewinfoavail;

bool MufFunctions::_hasnewinfo = false;
bool MufFunctions::_abort = false;

QList<QDir> MufFunctions::fnDirList{};
QList<MufFunctions::muf_function_t*> MufFunctions::fnList{};

MufExprtkBackend* MufFunctions::mExprtk = nullptr;

QString fnToken = "def";

MufFunctions::MufFunctions(
        const QStringList& dirList,
        MufExprtkBackend* exprtk,
        QObject* parent)
        : QObject(parent)
{
	mExprtk = exprtk;
	for (const auto& s : dirList) {
		fnDirList.append(QDir(s));
		loadDir(fnDirList.back());
	}
}

MufFunctions::~MufFunctions()
{
	for (auto& fn : fnList) {
		delete fn;
		fn = nullptr;
	}
	fnList.clear();
}

bool
MufFunctions::loadDir(const QDir& dir)
{
//	for (QString s : dir.entryList()) {
//		loadFile(QDir(dir.absoluteFilePath(s)));
//	}
//	qDebug() << dir.absolutePath();
	for (auto& fn : dir.entryInfoList()) {
		if (fn.isFile()) {
			QFile func(fn.absoluteFilePath());
			func.open(QIODevice::ReadOnly);
			QTextStream is(&func);
			QStringList body{};
			QString line;
			while (is.readLineInto(&line)) {
				body.append(line);
			}
			loadFile(fn, body);
			func.close();
		} else if (fn.isDir()) {
			if (fn.fileName() == "." or
			    fn.fileName() == "..") {
				continue;
			}
			loadDir(QDir(fn.filePath()));
//			qDebug() << fn.filePath();
		}
	}
	return true;
}

bool
MufFunctions::loadFile(
        const QFileInfo& fileInfo,
        const QStringList& fileBody)
{
//	qDebug() << fileInfo << fileBody;

	loadFunctions(fileInfo, fileBody);
	// TODO: manage conflicts
	return true;
}

bool
MufFunctions::loadFunctions(
        const QFileInfo& fileInfo,
        const QStringList& fileBody)
{
	muf_function_t* fn;
	for (QString line : fileBody) {
		if (line.startsWith(fnToken)) {
//			qDebug() << line;
			line.remove(0, 4);
			QStringList def = line.split('(');
			QString fname = def.at(0);
			QString arg = def.at(1);
			arg.chop(2);
			QStringList args = arg.split(", ");
//			qDebug() << "hopefully the fn name:" << fname;
//			qDebug() << "with args:" << args;
			fn = new muf_function_t();
			fnList.append(fn);
			fn->args = args;
			fn->name = fname;
			fn->fileInfo = fileInfo;
		} else if (line.startsWith('\t')) {
//			qDebug() << line;
			line.remove(0, 1);
			fn->body += line;
		}
	}

	for (auto& fn : fnList) {
//		qDebug() << fn->fileInfo;
//		qDebug() << fn->name << fn->args << fn->body;
		mExprtk->addFunction(fn->name, fn->body, fn->args);
	}

	return true;
}
