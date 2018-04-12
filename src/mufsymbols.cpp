#include "mufsymbols.h"

#include <QDebug>

MufSymbols::MufSymbols(QObject* parent)
        : QObject(parent)
{
	init();
}

void MufSymbols::display(QString s)
{
	qDebug() << "Text  |" << s;
	s = "";
	for (auto el : stack) {
		s.append(el.s);
	}
	qDebug() << "Stack |" << s;
	s = "";
	for (auto el : queue) {
		s.append(el.s);
	}
	qDebug() << "Queue |" << s;
	qDebug() << "----------------------------";
}


int prec_booster;

#if defined(__GNUG__)
        #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

int MufSymbols::init()
{
	pat_eos = {"", Assoc::NONE, 0};
	pat_ops = {
	        {"^\\)",        Assoc::NONE, -1},
	        {"^\\*\\*",     Assoc::RIGHT, 4},
	        {"^\\^",        Assoc::RIGHT, 4},
	        {"^\\*",        Assoc::RIGHT, 3},
	        {"^/",          Assoc::RIGHT, 3},
	        {"^\\+",        Assoc::RIGHT, 2},
	        {"^-",          Assoc::RIGHT, 2}
	};
	pat_arg = {
	        {"^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?"},
	        {"^[a-zA-Z_][a-zA-Z_0-9]*"},
	        {"^\\(", Assoc::LEFT, -1}
	};

	for (auto& el : pat_ops) {
		el.re.setPattern(el.str);
		if (!el.re.isValid()) {
			qDebug() << "regex invalid" << el.str;
			return 0;
		}
	}

	for (auto& el : pat_arg) {
		el.re.setPattern(el.str);
		if (!el.re.isValid()) {
			qDebug() << "regex invalid" << el.str;
			return 0;
		}
	}

	return 1;
}

#if defined(__clang__)
        #pragma clang diagnostic pop
#endif
#if defined(__GNUG__)
        #pragma GCC diagnostic warning "-Wmissing-field-initializers"
#endif

MufSymbols::pat_t*
MufSymbols::match(
        QString s,
        QList<MufSymbols::pat_t> p,
        MufSymbols::str_tok_t* t,
        QString* e)
{
	QRegularExpressionMatch m;

//	qDebug() << "orig s:" << s;
	while (s.startsWith(' ')) {
		s.remove(0, 1);
	}
//	qDebug() << "new s:" << s;
	*e = s;

//	qDebug() << s;
//	qDebug() << *e;

	if (s.size() == 0) {
		return &pat_eos;
	}

//	for (i = 0; p[i].str; i++) {
	for (auto& el : p) {
//		if (regexec(&(p[i].re), s, 1, &m, REG_NOTEOL))
		m = el.re.match(s);
		if (!m.hasMatch()) {
//			qDebug() << "no match";
			continue;
		}
		t->s = m.captured(0);
		t->len = m.capturedLength(0);
		*e = s.mid(t->len);
		return &el;
	}
	return 0;
}

int
MufSymbols::parse(QString s)
{
	pat_t* p;
	str_tok_t* t, tok;

	prec_booster = 0;

//	qDebug().nospace() << "string\n" << s;
//	qDebug() << "stack:";
//	for (auto el : stack) {
//		qDebug() << el.s;
//	}
//	qDebug() << "queue:";
//	for (auto el : queue) {
//		qDebug() << el.s;
//	}
	display(s);

	while (!s.isEmpty()) {
		p = match(s, pat_arg, &tok, &s);
		if (!p || p == &pat_eos) {
//			qDebug() << "p: " << p;
			qDebug() << "parse arg" << s;
			return 0;
		}

		/* Odd logic here. Don't actually stack the parens: don't need to. */
		if (p->prec == -1) {
			prec_booster += 100;
			continue;
		}
		queue.push_back(tok);
//		qDebug().nospace() << "string\n" << s;
//		qDebug() << "stack:";
//		for (auto el : stack) {
//			qDebug() << el.s;
//		}
//		qDebug() << "queue:";
//		for (auto el : queue) {
//			qDebug() << el.s;
//		}
		display(s);

re_op:
		p = match(s, pat_ops, &tok, &s);
		if (!p) {
			qDebug() << "parse arg" << s;
			return 0;
		}

		tok.assoc = p->assoc;
		tok.prec = p->prec;
//		qDebug() << p->prec;

		if (p->prec > 0) {
			tok.prec = p->prec + prec_booster;
		} else if (p->prec == -1) {
			if (prec_booster < 100) {
				qDebug() << "unmatched )" << s;
				return 0;
			}
			tok.prec = prec_booster;
		}

		while (!stack.empty()) {
			t = &stack.back();
			if (!(t->prec == tok.prec && t->assoc == Assoc::LEFT) &&
			    t->prec <= tok.prec) {
//				qDebug() << "broke with:";
//				qDebug() << "stack.size():" << stack.size() ;
//				qDebug() << "t->assoc:" << t->assoc;
//				qDebug() << "t->prec:" << t->prec;
//				qDebug() << "tok.prec:" << tok.prec;
				break;
			}
			queue.push_back(stack.pop());
//			qDebug().nospace() << "string\n" << s;
//			qDebug() << "stack:";
//			for (auto el : stack) {
//				qDebug() << el.s;
//			}
//			qDebug() << "queue:";
//			for (auto el : queue) {
//				qDebug() << el.s;
//			}
			display(s);
		}

		if (p->prec == -1) {
			prec_booster -= 100;
			goto re_op;
		}

		if (!p->prec) {
//			qDebug().nospace() << "string\n" << s;
//			qDebug() << "stack:";
//			for (auto el : stack) {
//				qDebug() << el.s;
//			}
//			qDebug() << "queue:";
//			for (auto el : queue) {
//				qDebug() << el.s;
//			}
			display(s);

			if (prec_booster) {
				qDebug() << "unmatched (" << s;
				return 0;
			}
			return 1;
		}

		stack.push(tok);
//		qDebug().nospace() << "string\n" << s;
//		qDebug() << "stack:";
//		for (auto el : stack) {
//			qDebug() << el.s;
//		}
//		qDebug() << "queue:";
//		for (auto el : queue) {
//			qDebug() << el.s;
//		}
		display(s);
	}

//	for (auto el : queue) {
//		qDebug() << el.s;
//	}
	return 1;
}
