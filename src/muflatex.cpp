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
	qDebug() << input ;

	return input;
}

