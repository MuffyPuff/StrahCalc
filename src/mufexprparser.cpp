#include "mufexprparser.h"

#include <QDebug>

MufExprParser::MufExprParser(QObject* parent) : QObject(parent)
{
	init();
}

#if defined(__GNUG__)
        #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(__clang__)
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

MufExprParser::str_tok_t MufExprParser::tok_end = {"end", Assoc::NONE, 0, TokenType::NONE};
MufExprParser::str_tok_t MufExprParser::op_sent = {"sentinel", Assoc::NONE, 0, TokenType::NONE};

int MufExprParser::init()
{
	pat_eos = {""};
	pat_ops = {
	        {"^[)}\\]]", TokenType::b, Assoc::RIGHT,  -1},
	        {"^\\^",     TokenType::B, Assoc::RIGHT,   7},
	        {"^\\*",     TokenType::B, Assoc::LEFT,    6},
	        {"^\\/",     TokenType::B, Assoc::LEFT,    6},
	        {"^\\%",     TokenType::B, Assoc::LEFT,    6},
	        {"^\\!",     TokenType::B, Assoc::POSTFIX, 5},
	        {"^\\+",     TokenType::B, Assoc::LEFT,    4},
	        {"^\\-",     TokenType::B, Assoc::LEFT,    4},
	        {"^\\=",     TokenType::B, Assoc::LEFT,    3},
	        {"^\\&\\&",  TokenType::B, Assoc::LEFT,    2},
	        {"^\\|\\|",  TokenType::B, Assoc::LEFT,    1}
	};
	pat_arg = {
	        {"^[-+]", TokenType::U, Assoc::PREFIX, 4},
	        {"^[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", TokenType::v, Assoc::NONE, 0},
	        {"^[a-zA-Z_][a-zA-Z_0-9]*", TokenType::v, Assoc::NONE, 0},
	        {"^[({\\[]", TokenType::b, Assoc::LEFT, -1}
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

	mOperators.push(op_sent);

	return 1;
}

#if defined(__clang__)
        #pragma clang diagnostic pop
#endif
#if defined(__GNUG__)
        #pragma GCC diagnostic warning "-Wmissing-field-initializers"
#endif

bool
MufExprParser::expect(const QString& tok_s)
{
	if (next().s == tok_s) {
		consume();
		return true;
	}
	return false;
}

bool
MufExprParser::expect(const str_tok_t& tok)
{
	if (next() == tok) {
		consume();
		return true;
	} else {
		return false;
	}
	Q_UNREACHABLE();
}

bool
MufExprParser::expect(const MufExprParser::TokenType& tok_t)
{
	if (next().type == tok_t) {
		consume();
		return true;
	} else {
		return false;
	}
}

void
MufExprParser::consume()
{
	if (!tokens.isEmpty()) {
		tokens.pop_front();
	}
}

MufExprParser::str_tok_t
MufExprParser::next()
{
	if (!tokens.isEmpty()) {
		return tokens.first();
	} else {
		return tok_end;
	}
	Q_UNREACHABLE();
}

bool
MufExprParser::exprRecognise(QString input)
{
	if (tokenize(input)) {
		if (exprR() and expect(tok_end)) {
			return true;
		}
	}
	qDebug() << "tokens:" << input;
	for (auto el : tokens) {
		qDebug() << el.s << el.type;
	}
	qDebug() << "not valid";
	return false;
}

QString
MufExprParser::exprParse(QString input)
{
	mOperands.clear();
	mOperators.clear();
	mOperators.push(op_sent);
	tokens.empty();
	if (tokenize(input)) {
		if (expr() and expect(tok_end)) {
			qDebug() << "tree:" << input;
			return mOperands.top().print();
		}
	}
	qDebug() << "tokens:" << input;
	for (auto el : tokens) {
		qDebug() << el.s << el.type;
	}
	return "not valid";
}

bool
MufExprParser::tokenize(QString input)
{
	pat_t* p;
	str_tok_t tok;

	while (!input.isEmpty()) {
re_val:
		p = match(input, pat_arg, &tok, &input);
		if (!p || p == &pat_eos) {
			qDebug() << "parse arg" << input;
			return 0;
		}
		/*
		if (tok.s.at(0) == "-" or tok.s.at(0) == "+") {
			str_tok_t un;
			un.s = tok.s.at(0);
			un.type = TokenType::U;
			un.assoc = Assoc::PREFIX;
			un.prec = 4;
			tokens.push_back(un);
			tok.s.remove(0, 1);
		}
		*/
		tokens.push_back(tok);
//		qDebug() << tok.s << tok.type << tok.assoc << input;
		if ((tok.type == TokenType::U and tok.assoc == Assoc::PREFIX) or
		    (tok.type == TokenType::b and tok.assoc == Assoc::LEFT)) {
			goto re_val;
		}
re_op:
		p = match(input, pat_ops, &tok, &input);
		if (!p) {
			qDebug() << "parse arg" << input;
			return 0;
		}

//		tok.assoc = p->assoc;
//		tok.prec = p->prec;
		if (!p->prec) {
			return 1;
		}

		tokens.push_back(tok);
		if (tok.type == TokenType::b and tok.assoc == Assoc::RIGHT) {
			goto re_op;
		}
	}
	return 1;
}

MufExprParser::pat_t*
MufExprParser::match(
        QString s,
        QList<MufExprParser::pat_t> p,
        MufExprParser::str_tok_t* t,
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
		t = &tok_end;
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
		t->type = el.type;
		t->assoc = el.assoc;
		t->prec = el.prec;
		*e = s.mid(m.capturedLength(0));
		return &el;
	}
	return 0;
}

bool
MufExprParser::expr()
{
	if (!p()) {
		qDebug() << "first p";
		return false;
	}
	while (next().type == TokenType::B) {
//		Q_UNIMPLEMENTED(); // push next
		pushOperator(next());
		consume();
		if (!p()) {
			qDebug() << "second p";
			return false;
		}
	}
	while (mOperators.top() != op_sent) {
//		Q_UNIMPLEMENTED(); // pop
		popOperator();
	}
	return true;
}

bool
MufExprParser::p()
{
//	qDebug() << "j";
	if (next().type == TokenType::v) {
//		qDebug() << "val";
		mOperands.push(ExprTree(next()));
//		qDebug() << "push?";
		consume();
	} else if (next().type == TokenType::b and next().assoc == Assoc::LEFT) {
//		qDebug() << "bin";
		QChar b = next().s.at(0); // we know it is length 1
		consume();
		mOperators.push(op_sent);
		if (!expr()) {
			return false;
		}
		b = b.toLatin1() + 1 + (b != '('); // add 1 and 1 more for { and [
		if (!expect(b)) {
			qDebug() << "expected" << b << "found:" << tokens.first().s;
			return false;
		}
		mOperators.pop();
	} else if (next().type == TokenType::U and next().assoc == Assoc::PREFIX) {
//		qDebug() << "un";
//		Q_UNIMPLEMENTED(); // push next
		pushOperator(next());
		consume();
		if (!p()) {
			qDebug() << "expected value";
			return false;
		}
	} else {
		qDebug() << "unexpected state";
		return false;
	}
	return true;
}

void
MufExprParser::pushOperator(const MufExprParser::str_tok_t& op)
{
//	qDebug() << "pushOperator";
	while (mOperators.top() > op) {
		popOperator();
	}
	mOperators.push(op);
}

void
MufExprParser::popOperator()
{
//	qDebug() << "popOperator";
	if (mOperators.top().type == TokenType::B) {
//		qDebug() << "bin";
		auto t1 = mOperands.pop();
		auto t0 = mOperands.pop();
		mOperands.push(ExprTree(mOperators.pop(), t0, t1));
	} else {
//		qDebug() << "un";
		mOperands.push(ExprTree(mOperators.pop(), ExprTree(mOperands.pop())));
	}
}

bool
MufExprParser::exprR()
{
	if (!p()) {
		qDebug() << "first p";
		return false;
	}
	while (next().type == TokenType::B) {
		consume();
		if (!p()) {
			qDebug() << "second p";
			return false;
		}
	}
	return true;
}

bool
MufExprParser::pR()
{
	if (next().type == TokenType::v) {
		consume();
	} else if (next().type == TokenType::b and next().assoc == Assoc::LEFT) {
		QChar b = next().s.at(0); // we know it is length 1
		consume();
		if (!exprR()) {
			return false;
		}
		b = b.toLatin1() + 1 + (b != '('); // add 1 and 1 more for { and [
		if (!expect(b)) {
			qDebug() << "expected" << b << "found:" << tokens.first().s;
			return false;
		}
	} else if (next().type == TokenType::U and next().assoc == Assoc::PREFIX) {
		consume();
		if (!pR()) {
			qDebug() << "expected value";
			return false;
		}
	} else {
		qDebug() << "unexpected state";
		return false;
	}
	return true;
}

bool
operator>(
        const MufExprParser::str_tok_t& lhs,
        const MufExprParser::str_tok_t& rhs)
{
	if (lhs.type == MufExprParser::TokenType::B and
	    rhs.type == MufExprParser::TokenType::B) { // bin(x) > bin(y)
		if (lhs.prec > rhs.prec) {
			return true;
		} else {
			return false;
		}
	} else if (lhs.type == MufExprParser::TokenType::U and
	           rhs.type == MufExprParser::TokenType::B) { // un(x) > bin(x)
		if (lhs.prec >= rhs.prec) {
			return true;
		} else {
			return false;
		}
	} else if (rhs.type == MufExprParser::TokenType::U) { // op > unary(y)
		return false;
	} else if (lhs == MufExprParser::op_sent) { // sentinel > op
		return false;
	} else if (rhs == MufExprParser::op_sent) { //op > sentinel
		Q_UNREACHABLE();
		return true;
	} else {
		Q_ASSERT_X(false, "compare", "unexpected operands");
	}
	Q_UNREACHABLE();
}

bool
operator<(
        const MufExprParser::str_tok_t& lhs,
        const MufExprParser::str_tok_t& rhs)
{
	return rhs > lhs;
}

bool
operator>=(
        const MufExprParser::str_tok_t& lhs,
        const MufExprParser::str_tok_t& rhs)
{
	return not(lhs < rhs);
}

bool
operator<=(
        const MufExprParser::str_tok_t& lhs,
        const MufExprParser::str_tok_t& rhs)
{
	return not(lhs > rhs);
}

bool
operator==(
        const MufExprParser::str_tok_t& lhs,
        const MufExprParser::str_tok_t& rhs)
{
	return (lhs.s     == rhs.s    and
	        lhs.type  == rhs.type and
	        lhs.prec  == rhs.prec and
	        lhs.assoc == rhs.assoc);
}

bool
operator!=(
        const MufExprParser::str_tok_t& lhs,
        const MufExprParser::str_tok_t& rhs)
{
	return not(lhs == rhs);
}
