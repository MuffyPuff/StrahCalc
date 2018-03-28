#include "muflatex.h"

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

