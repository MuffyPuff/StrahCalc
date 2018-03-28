#ifndef GLOBAL_H
#define GLOBAL_H

#include <QSettings>

#include "muftranslate.h"
#include "muflatex.h"

#include <cmath>


#define ALL(c) (c).begin(),(c).end()
#define IN(x, c) (find(c.begin(), c.end(), x) != (c).end())
#define REP(i, n) for (int i = 0; i < (int)(n); ++i)
#define FOR(i, a, b) for (int i = (a); i <= (b); ++i)

namespace Muf
{

//static MufTranslate translation("en-GB");
static MufTranslate translation("sl-SI");
static MufLatex toLatex;

static int prec = 12;

static QString
roundFloat(const double& value, int count = 0)
{
	if (count > 15) {
		return QString::number(value, 'g', prec);
	}
	int whole = round(value);
	if (qFuzzyCompare(1 + whole, 1 + value)) {
		return QString::number(whole, 'g', prec);
	} else {
		return QString::number(
		               roundFloat(value * 10,
		                          count + 1).toDouble() * 0.1,
		               'g', prec);
	}
}

typedef double                                  num_t;
typedef std::string                             str_t;
typedef std::vector<num_t>                      vec_t;

//template<typename T>
//struct symbol_t {
//	symbol_t(str_t n, const T& v)
//	        : name(n), value(new T(v)) {}
//	str_t           name;
//	T               value;
//};


}


#endif // GLOBAL_H
