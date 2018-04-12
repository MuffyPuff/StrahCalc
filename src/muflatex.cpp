#include "muflatex.h"

#include <QRegularExpression>

#include <QDebug>

MufLatex::MufLatex(QObject* parent)
        : QObject(parent)
{
}

QString
MufLatex::operator()(QString input)
{
	input = input.split(';').last();
	input = assignment(input);

	input = mPar(input);
//	mPar.reduce = true;
	input.append("&=");
	mPar.tree->reduce();
	mPar.tree->toFrac();
	mPar.tree->reduce();
	input.append(mPar.tree->toLatex());

//	input = power(input);
//	input = operators(input);
//	input = braces(input);
	qDebug() << input ;

	return input;
}

QString
MufLatex::assignment(QString input)
{
	input = input.split(":=").first();
	input = input.split("+=").first();
	input = input.split("-=").first();
	input = input.split("*=").first();
	input = input.split("/=").first();

	return input;
}

QString
MufLatex::power(QString input)
{
	if (input.indexOf("^") == -1) {
		return input;
	}
//	input = input.replace(QRegExp("(.+)\\^\\(([^\\)]+)\\)(.*)"),
//	input = input.replace(QRegExp("(.+)\\(((?:[^()]|(?R))*)\\)(.*)"),
//	                        "\\1{\\2}\\3");


//	QRegularExpression pow("\\^\\(((?:[^()]|(?R))*)\\)");
//	QRegularExpression pow("(.+\\^)|\\(((?:[^()]|(?R))*)\\)");
//	QRegularExpression pow("([^()]+?)(?:\\(((?:[^()]|(?R))*)\\))");
	QRegularExpression
	pow("(?<base>[^^]+?)\\^(?<recurse>\\((?<exp>(?:[^()]|\\g<recurse>)+)\\))");
//	auto i = pow.globalMatch(input);
//	while (i.hasNext()) {
//		auto match = i.next();
////            qDebug() << match.lastCapturedIndex();
//		words << match.captured(1);
//	}
	auto m = pow.match(input);
	QString exp = m.captured("exp");
//	QString res = (exp.indexOf("^") != -1) ? power(exp) : exp;
	if (m.hasMatch()) {
		input.replace(m.captured("recurse"), "{" + power(exp) + "}");
	}
//	QStringList words;
//	auto i = base.globalMatch(input);
//	while (i.hasNext()) {
//		auto match = i.next();
////            qDebug() << match.lastCapturedIndex();
//		words << match.captured(1);
//	}
//	i = exp.globalMatch(input);
//	while (i.hasNext()) {
//		auto match = i.next();
//		words << match.captured(1);
//	}
//	i = tail.globalMatch(input);
//	while (i.hasNext()) {
//		auto match = i.next();
//		words << match.captured(1);
//	}
//	input = words.at(0) + "{" +
//	         words.at(1) + "}" +
//	         words.at(2);

//	input = input.replace(pow, "\\1^{\\3}");
//	input = input.replace("{}", "");

////    std::regex pow("\\(((?:[^\\(\\)]|(?R))*)\\)");
//	std::regex pow("(\\((.+)\\))");
//	std::cmatch cm;
////    std::regex_match(input.toStdString().c_str(), cm, pow);
//	QString tmp = "none";
//	auto s = input.toStdString();
//	while (std::regex_search(s.c_str(), cm, pow)) {
//		for (auto x : cm) {
//			qDebug() << x << " ";
//		}
//		s = cm.suffix().str();
//	}
//	qDebug() << cm.size();
//	qDebug() << input ;//<< " -> " << tmp;
	return input;
}

QString
MufLatex::operators(QString input)
{
	input.replace("*", "\\cdot");
	input.replace("/", "\\div");
	input.replace("%", "\\%");

	return input;
}

QString
MufLatex::braces(QString input)
{
	input.replace("(", " \\left(");
	input.replace(")", " \\right)");

	return input;
}

