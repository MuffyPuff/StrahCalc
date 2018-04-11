#include "mufexprparser.h"

#include <QDebug>
#include <QHash>

#include <algorithm>
#include <cmath>

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

MufExprParser::str_tok_t MufExprParser::tok_end = {
        "end",
        MufExprParser::Assoc::NONE,
        0, 0, 0,
        MufExprParser::TokenType::NONE
};
MufExprParser::str_tok_t MufExprParser::op_sent = {
        "sentinel",
        MufExprParser::Assoc::NONE,
        0, 0, 0,
        MufExprParser::TokenType::NONE
};

int MufExprParser::init()
{
	pat_eos = {""};
	pat_ops = {
	        {"^[)}\\]]", TokenType::b, Assoc::RIGHT,  -1, 0, -2},
	        {"^\\^",     TokenType::B, Assoc::RIGHT,   7, 7, 6},
	        {"^\\!",     TokenType::B, Assoc::POSTFIX, 6, 0, 7},
	        {"^\\*",     TokenType::B, Assoc::LEFT,    5, 6, 5},
	        {"^\\/",     TokenType::B, Assoc::LEFT,    5, 6, 5},
	        {"^\\%",     TokenType::B, Assoc::LEFT,    5, 6, 5},
	        {"^\\+",     TokenType::B, Assoc::LEFT,    4, 5, 4},
	        {"^\\-",     TokenType::B, Assoc::LEFT,    4, 5, 4},
	        {"^\\=",     TokenType::B, Assoc::LEFT,    3, 4, 2},
	        {"^\\&\\&",  TokenType::B, Assoc::LEFT,    2, 3, 2},
	        {"^\\|\\|",  TokenType::B, Assoc::LEFT,    1, 2, 1}
	};
	pat_arg = {
	        {"^[-+]", TokenType::U, Assoc::PREFIX, 6, 0, 0},
	        {"^[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?", TokenType::v, Assoc::NONE, 0},
	        {"^[a-zA-Z_][a-zA-Z_0-9]*", TokenType::x, Assoc::NONE, 0},
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

//	mOperators.push(op_sent);
	tree = nullptr;

//	qDebug("init done");

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
MufExprParser::exprParseSY(QString input)
{
	mOperands.clear();
	mOperators.clear();
	mOperators.push(op_sent);
	tokens.clear();

	if (tokenize(input)) {
		if (exprSY() and expect(tok_end)) {
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

QString MufExprParser::exprParseRD(QString input)
{
	if (tree != nullptr) {
		delete tree;
		tree = nullptr;
	}
	ExprTree* t;
	tokens.clear();
	if (tokenize(input)) {
		t = exprRD(0);
		if (t != nullptr and expect(tok_end)) {
			tree = t;
			qDebug() << "tree:" << input;
			return tree->print();
		}
	}
	qDebug() << "tokens:" << input;
	for (auto el : tokens) {
		qDebug() << el.s << el.type;
	}
	return "not valid";
}

void
traverse(MufExprParser::ExprTree* t)
{
	qDebug() << "Text  |" << t->op.s;
	qDebug() << "Neg?  |" << t->negative();
	qDebug() << "Val?  |" << t->isValue();
	qDebug() << "Val   |" << t->eval();
	qDebug() << "Odd?  |" << t->isOdd();
	qDebug() << "Even? |" << t->isEven();
	qDebug() << "Frac? |" << t->isFrac();
	qDebug() << "----------------------------";
	for (const auto& el : t->operands) {
		traverse(el);
	}
}

QString
MufExprParser::exprParseTD(QString input)
{
	if (tree != nullptr) {
		delete tree;
		tree = nullptr;
	}
	ExprTree* t;
	tokens.clear();
	if (tokenize(input)) {
		t = exprTD(0);
		if (t != nullptr and expect(tok_end)) {
			tree = t;
			// TODO: remove qDebugs
//			qDebug() << "tree:" << input;
//			qDebug() << tree->print();
//			tree->reduce();
//			qDebug() << "reduced tree:";
//			return tree->print();
//			traverse(tree);
			t->numeric = true;
			// TODO: set numeric to childs
			tree->reduce();
			tree->toFrac();
			tree->reduce();
//			qDebug() << "isFraction: " << tree->isFrac();
//			qDebug() << tree->print();
//			traverse(tree);
			qDebug() << tree->toLatex();
			return tree->toLatex();
		}
//		qDebug() << "expr or expect" << tokens.size();
	}
	qDebug() << "tokens:" << input;
	for (auto el : tokens) {
		qDebug() << el.s << el.type;
	}
	return "not valid";
}

QString
MufExprParser::operator()(QString input)
{
//	if (tree != nullptr) {
//		delete tree;
//		tree = nullptr;
//	}
//	ExprTree* t;
//	tokens.clear();
//	if (tokenize(input)) {
//		t = exprTD(0);
//		if (t != nullptr and expect(tok_end)) {
//			tree = t;
////                    qDebug() << "tree:" << input;
//			return true;
//		}
//	}
//	qDebug() << "tokens:" << input;
//	for (auto el : tokens) {
//		qDebug() << el.s << el.type;
//	}
	return exprParseTD(input);
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
		t->type         = el.type;
		t->assoc        = el.assoc;
		t->prec         = el.prec;
		t->rightPrec    = el.rightPrec;
		t->nextPrec     = el.nextPrec;
		*e = s.mid(m.capturedLength(0));
		return &el;
	}
	return 0;
}

MufExprParser::ExprTree*
MufExprParser::exprTD(int p)
{
	ExprTree* t = pTD();
	if (t == nullptr) {
		qDebug() << "first p";
		return nullptr;
	}
	qint8 r = 7;
	while ((next().type == TokenType::B or
	        next().assoc == Assoc::POSTFIX) and
	       next().prec >= p and
	       next().prec <= r) {
		str_tok_t op = next();
		consume();
		if (op.type == TokenType::B) {
			ExprTree* t1 = exprTD(op.rightPrec);
			t = new ExprTree(op, t, t1);
		} else {
			t = new ExprTree(op, t);
		}
		r = op.nextPrec;
	}
	return t;
}

MufExprParser::ExprTree*
MufExprParser::pTD()
{
	if (next().type == TokenType::v or
	    next().type == TokenType::x) {
		ExprTree* t = new ExprTree(next());
		consume();
		return t;
	} else if (next().type == TokenType::b and next().assoc == Assoc::LEFT) {
		QChar b = next().s.at(0); // we know it is length 1
		consume();
		ExprTree* t = exprTD(0);
		b = b.toLatin1() + 1 + (b != '('); // add 1 and 1 more for { and [
		if (!expect(b)) {
			qDebug() << "expected" << b << "found:" << tokens.first().s;
			return nullptr;
		}
		return t;
	} else if (next().type == TokenType::U and next().assoc == Assoc::PREFIX) {
		str_tok_t op = next();
		consume();
//		qint8 q = op.prec;
		ExprTree* t = exprTD(6); // for "/[*/%]/"? idk
		return new ExprTree(op, t);
	} else {
		qDebug() << "unexpected state";
		return nullptr;
	}
	Q_UNREACHABLE();
}

MufExprParser::ExprTree*
MufExprParser::exprRD(int p)
{
	ExprTree* t = pRD();
	if (t == nullptr) {
		qDebug() << "first p";
		return nullptr;
	}
	while (next().type == TokenType::B and
	       next().prec >= p) {
		str_tok_t op = next();
		consume();
		qint8 q = 0;
		if (op.assoc == Assoc::RIGHT) {
			q = op.prec;
		} else if (op.assoc == Assoc::LEFT) {
			q = 1 + op.prec;
		}
		ExprTree* t1 = exprRD(q);
		t = new ExprTree(op, t, t1);
	}
	return t;
}

MufExprParser::ExprTree*
MufExprParser::pRD()
{
	if (next().type == TokenType::v or
	    next().type == TokenType::x) {
		ExprTree* t = new ExprTree(next());
		consume();
		return t;
	} else if (next().type == TokenType::b and next().assoc == Assoc::LEFT) {
		QChar b = next().s.at(0); // we know it is length 1
		consume();
		ExprTree* t = exprRD(0);
		b = b.toLatin1() + 1 + (b != '('); // add 1 and 1 more for { and [
		if (!expect(b)) {
			qDebug() << "expected" << b << "found:" << tokens.first().s;
			return nullptr;
		}
		return t;
	} else if (next().type == TokenType::U and next().assoc == Assoc::PREFIX) {
		str_tok_t op = next();
		consume();
		qint8 q = op.prec;
		ExprTree* t = exprRD(q);
		return new ExprTree(op, t);
	} else {
		qDebug() << "unexpected state";
		return nullptr;
	}
	Q_UNREACHABLE();
}

bool
MufExprParser::exprSY()
{
	if (!pSY()) {
		qDebug() << "first p";
		return false;
	}
	while (next().type == TokenType::B) {
		pushOperator(next());
		consume();
		if (!pSY()) {
			qDebug() << "second p";
			return false;
		}
	}
	while (mOperators.top() != op_sent) {
//		qDebug() << "this?";
		popOperator();
//		qDebug() << "this?";
	}
	return true;
}

bool
MufExprParser::pSY()
{
//	qDebug() << "j";
	if (next().type == TokenType::v or
	    next().type == TokenType::x) {
//		qDebug() << "val";
		mOperands.push(ExprTree(next()));
//		qDebug() << "push?";
		consume();
	} else if (next().type == TokenType::b and next().assoc == Assoc::LEFT) {
//		qDebug() << "bin";
		QChar b = next().s.at(0); // we know it is length 1
		consume();
		mOperators.push(op_sent);
		if (!exprSY()) {
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
		pushOperator(next());
		consume();
		if (!pSY()) {
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
//		qDebug("this2?");
		mOperands.push(ExprTree(mOperators.pop(), t0, t1));
//		qDebug("this3?");
	} else {
//		qDebug() << "un";
		mOperands.push(ExprTree(mOperators.pop(), ExprTree(mOperands.pop())));
	}
}

bool
MufExprParser::exprR()
{
	if (!pR()) {
		qDebug() << "first p";
		return false;
	}
	while (next().type == TokenType::B) {
		consume();
		if (!pR()) {
			qDebug() << "second p";
			return false;
		}
	}
	return true;
}

bool
MufExprParser::pR()
{
	if (next().type == TokenType::v or
	    next().type == TokenType::x) {
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

// function from https://stackoverflow.com/a/2112111/1150303
int constexpr
chash(const char* input)
{
	return *input ?
	       static_cast<unsigned int>(*input) + 33 * chash(input + 1) :
	       5381;
}

int
chash(const QString& input)
{
//	qDebug("overload for chash");
	return chash(input.toStdString().c_str());
}

int
MufExprParser::ExprTree::negative()
{
	if (op.type == TokenType::v or
	    op.type == TokenType::x or
	    operands.isEmpty()) { // no operands but not a value
		return 1;
	}

	switch (chash(this->op.s)) {
	case chash("-"): {
		if (this->op.type == TokenType::U) {
			return -this->operands.first()->negative();
		} else {
			int n1 = this->operands.first()->negative();
			int n2 = this->operands.last()->negative();
			return n1 - n2;
			return 0;
		}
		break;
	}
	case chash("+"): {
		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		return n1 + n2;
		break;
	}
	case chash("/"): {
		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		return n1 / n2;
//		if (n1 >= 0 and n2 <  0 or
//		    n1 <  0 and n2 >= 0) {
//			return -min(abs(n1), abs(n2));
//		}
//		if (n1 <  0 and n2 <  0 or
//		    n1 >= 0 and n2 >= 0) {
//			return min(abs(n1), abs(n2));
//		}
//		return n1 + n2;
//		bool b1 = this->operands.first()->negative();
//		bool b2 = this->operands.last()->negative();
//		if (b1 ^ b2) {
//			return true;
//		} else {
//			return 0;
//		}
		break;
	}
	case chash("*"): {
		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		return n1 * n2;
		break;
	}
	case chash("^"): {
		int n = this->operands.first()->negative();
		if (this->operands.last()->isValue() and
		    this->operands.last()->isEven()) {
			return abs(n);
		}
		if (this->operands.last()->isValue() and
		    this->operands.last()->isOdd()) {
			return n;
		}
		return 1; // if exponent is not a value we can't tell parity
//		bool b1 = this->operands.first()->negative();
//		bool b2 = this->operands.last()->isValue();
//		bool b3 = this->operands.last()->isOdd();
//		if (b1 & b2 & b3) {
//			return true;
//		} else {
//			return 0;
//		}
		break;
	}
	case chash("%"):
		return 1;
		break;
	default:
		Q_UNIMPLEMENTED();
		return 0;
		break;
	}
	Q_UNREACHABLE();
}

int gcd(int a, int b)
{
	if (b == 0) {
		return a;
	} else {
		return gcd(b, a % b);
	}
}

bool coprime(int a, int b)
{
	if (gcd(a, b) == 1) {
		return true;
	} else {
		return false;
	}
}


bool
MufExprParser::ExprTree::isValue()
{
	if (this->op.type == TokenType::v) {
		return true;
	}
	if (this->op.type == TokenType::x) {
		return false;
	}
	if (this->operands.isEmpty()) { // no operands but not a value
		return false;
	}
	// TODO: move to isInt
	if (this->op.type == TokenType::B and
	    this->op.s == "/" and
	    this->operands.first()->isValue() and
	    this->operands.last()->isValue()) {
		return ((int)this->operands.first()->eval() %
		        (int)this->operands.last()->eval()) == 0;
	}
	for (auto& el : this->operands) {   // if any operand
		if (!el->isValue()) {       // is not a value
			return false;       // then this is not a value
		}
	}
	return true; // might be value
}

bool
MufExprParser::ExprTree::isFrac()
{
	if (this->op.type == TokenType::B and
	    this->op.s == "/") {
		return true;
	}
	return false;
}

bool
MufExprParser::ExprTree::hasFrac()
{
	if (this->op.type == TokenType::v) {
		return false;
	}
	if (this->op.type == TokenType::x) {
		return false;
	}
	if (this->operands.isEmpty()) { // no operands
		return false;
	}
	for (auto& el : operands) {
		if (el->isFrac()) {
			return true;
		}
	}
	return false;
}

void
MufExprParser::ExprTree::toFrac()
{
//	if (!this->hasFrac()) {
//		return;
//	}
	if (this->op.type == TokenType::v or
	    this->op.type == TokenType::x) {
		ExprTree* ttree = new ExprTree(*this);

		this->op.s         = "/";
		this->op.type      = TokenType::B;
		this->op.assoc     = Assoc::LEFT;
		this->op.prec      = 5; // raw assoc TODO: enum
		this->op.rightPrec = 6;
		this->op.nextPrec  = 5;

		this->operands.clear();
		this->operands.append(ttree);
		this->operands.append(new ExprTree());
		this->operands.last()->setValue("1");
		return;
	}
	if (operands.isEmpty()) { // no operands
		return;
	}
	if (op.type == TokenType::U and
	    op.assoc == Assoc::PREFIX and
	    op.s == "-") {
		ExprTree* ttree = new ExprTree(*this);

		this->op.s         = "/";
		this->op.type      = TokenType::B;
		this->op.assoc     = Assoc::LEFT;
		this->op.prec      = 5; // raw assoc TODO: enum
		this->op.rightPrec = 6;
		this->op.nextPrec  = 5;

		this->operands.clear();
		this->operands.append(ttree);
		this->operands.append(new ExprTree());
		this->operands.last()->setValue("1");
		return;
	}
	ExprTree* t1 = nullptr;
	ExprTree* t2 = nullptr;
	if (this->operands.first()->op.s == "^") {
		t1 = this->operands.first();
	}
	if (this->operands.last()->op.s == "^") {
		t2 = this->operands.last();
	}
	if (t1 != nullptr and t2 != nullptr) {
		ExprTree* ttree = new ExprTree(*this);

		this->op.s         = "/";
		this->op.type      = TokenType::B;
		this->op.assoc     = Assoc::LEFT;
		this->op.prec      = 5; // raw assoc TODO: enum
		this->op.rightPrec = 6;
		this->op.nextPrec  = 5;

		this->operands.clear();
		this->operands.append(ttree);
		this->operands.append(new ExprTree());
		this->operands.last()->setValue("1");
		return;
	}

	switch (chash(this->op.s)) {
	case chash("/"): {
		// TODO: a/b / c/d = ad/bc
		this->operands.first()->toFrac();
		this->operands.last()->toFrac();
		bool b11 = this->operands.first()->operands.first()->isValue();
		bool b12 = this->operands.first()->operands.last()->isValue();
		bool b21 = this->operands.last()->operands.first()->isValue();
		bool b22 = this->operands.last()->operands.last()->isValue();
		// TODO: negative values
		int v11 = this->operands.first()->operands.first()->eval();
		int v12 = this->operands.first()->operands.last()->eval();
		int v21 = this->operands.last()->operands.first()->eval();
		int v22 = this->operands.last()->operands.last()->eval();
		QStringList x11 = this->operands.first()->operands.first()->var();
		QStringList x12 = this->operands.first()->operands.last()->var();
		QStringList x21 = this->operands.last()->operands.first()->var();
		QStringList x22 = this->operands.last()->operands.last()->var();
		int n1 = v11 * v22;
		int n2 = v12 * v21;

		this->operands.first()->setValue(n1);
		for (auto& el : x11) {
			this->operands.first()->multiply(el);
		}
		for (auto& el : x22) {
			this->operands.first()->multiply(el);
		}
		this->operands.last()->setValue(n2);
		for (auto& el : x12) {
			this->operands.last()->multiply(el);
		}
		for (auto& el : x21) {
			this->operands.last()->multiply(el);
		}
		break;
	}
	case chash("*"): {
		// TODO: handle x*y/z in reduce
		this->operands.first()->toFrac();
		this->operands.last()->toFrac();
		int v11 = this->operands.first()->operands.first()->eval();
		int v12 = this->operands.first()->operands.last()->eval();
		int v21 = this->operands.last()->operands.first()->eval();
		int v22 = this->operands.last()->operands.last()->eval();
		QStringList x11 = this->operands.first()->operands.first()->var();
		QStringList x12 = this->operands.first()->operands.last()->var();
		QStringList x21 = this->operands.last()->operands.first()->var();
		QStringList x22 = this->operands.last()->operands.last()->var();
		int n2 = v12 * v22;
		int n1 = v11 * v21;

		this->op.s         = "/";
		this->op.type      = TokenType::B;
		this->op.assoc     = Assoc::LEFT;
		this->op.prec      = 5; // raw assoc TODO: enum
		this->op.rightPrec = 6;
		this->op.nextPrec  = 5;

		this->operands.clear();
		this->operands.append(new ExprTree());
		this->operands.append(new ExprTree());
		this->operands.first()->setValue(n1);
		this->operands.last()->setValue(n2);

		for (auto& el : x11) {
			this->operands.first()->multiply(el);
		}
		for (auto& el : x21) {
			this->operands.first()->multiply(el);
		}

		for (auto& el : x12) {
			this->operands.last()->multiply(el);
		}
		for (auto& el : x22) {
			this->operands.last()->multiply(el);
		}
		break;
	}
	case chash("+"):
	case chash("-"): {
		// TODO: a/b + c/d = (ad + bc) / bd
		if (t1 != nullptr) {
			// a^b + c/d = (da^b + c)/d
			auto v21 = this->operands.last()->operands.first();
			int v22 = this->operands.last()->operands.last()->eval();
			QStringList x22 = this->operands.last()->operands.last()->var();
			t1->multiply(v22);
			for (auto& el : x22) {
				this->operands.first()->operands.first()->multiply(el);
			}
			ExprTree* ttree = new ExprTree(*this);

			this->op.s         = "/";
			this->op.type      = TokenType::B;
			this->op.assoc     = Assoc::LEFT;
			this->op.prec      = 5; // raw assoc TODO: enum
			this->op.rightPrec = 6;
			this->op.nextPrec  = 5;

			this->operands.clear();
			this->operands.append(ttree);
			this->operands.append(new ExprTree());
			this->operands.first()->operands.first() = t1;
			this->operands.first()->operands.last() = v21;
			this->operands.last()->setValue(v22);
			for (auto& el : x22) {
				this->operands.last()->multiply(el);
			}
//			qDebug() << this->print();
			break;
		}
		if (t2 != nullptr) {
			// a^b + c/d = (da^b + c)/d
			auto v11 = this->operands.first()->operands.first();
			int v12 = this->operands.first()->operands.last()->eval();
			QStringList x12 = this->operands.last()->operands.last()->var();
			t2->multiply(v12);
			for (auto& el : x12) {
				this->operands.first()->operands.first()->multiply(el);
			}

			ExprTree* ttree = new ExprTree(*this);

			this->op.s         = "/";
			this->op.type      = TokenType::B;
			this->op.assoc     = Assoc::LEFT;
			this->op.prec      = 5; // raw assoc TODO: enum
			this->op.rightPrec = 6;
			this->op.nextPrec  = 5;

			this->operands.clear();
			this->operands.append(ttree);
			this->operands.append(new ExprTree());
			this->operands.first()->operands.last() = t2;
			this->operands.first()->operands.first() = v11;
			this->operands.last()->setValue(v12);
			for (auto& el : x12) {
				this->operands.last()->multiply(el);
			}
			break;
		}
		this->operands.first()->toFrac();
		this->operands.last()->toFrac();
		bool b11 = this->operands.first()->operands.first()->isValue();
		bool b12 = this->operands.first()->operands.last()->isValue();
		bool b21 = this->operands.last()->operands.first()->isValue();
		bool b22 = this->operands.last()->operands.last()->isValue();

		int v11 = this->operands.first()->operands.first()->eval();
		int v12 = this->operands.first()->operands.last()->eval();
		int v21 = this->operands.last()->operands.first()->eval();
		int v22 = this->operands.last()->operands.last()->eval();
		QStringList x11 = this->operands.first()->operands.first()->var();
		QStringList x12 = this->operands.first()->operands.last()->var();
		QStringList x21 = this->operands.last()->operands.first()->var();
		QStringList x22 = this->operands.last()->operands.last()->var();
		int greatestd = gcd(v12, v22);
		int n2 = abs(v12 * v22) / greatestd;
		int n11 = v11 * v22 / greatestd;
		int n12 = v21 * v12 / greatestd;
//		qDebug() << this->print();
//		qDebug() << this->operands.first()->operands.first()->print();
//		qDebug() << x11;
//		qDebug() << this->operands.first()->operands.last()->print();
//		qDebug() << x12;
//		qDebug() << this->operands.last()->operands.first()->print();
//		qDebug() << x21;
//		qDebug() << this->operands.last()->operands.last()->print();
//		qDebug() << x22;

		ExprTree* ttree = new ExprTree(*this);

		this->op.s         = "/";
		this->op.type      = TokenType::B;
		this->op.assoc     = Assoc::LEFT;
		this->op.prec      = 5; // raw assoc TODO: enum
		this->op.rightPrec = 6;
		this->op.nextPrec  = 5;

		this->operands.clear();
		this->operands.append(ttree);
		this->operands.append(new ExprTree());
		this->operands.first()->operands.first()->setValue(n11);
		this->operands.first()->operands.last()->setValue(n12);
		this->operands.last()->setValue(n2);


		for (auto& el : x11) {
			this->operands.first()->operands.first()->multiply(el);
		}
		for (auto& el : x22) {
			this->operands.first()->operands.first()->multiply(el);
		}

		for (auto& el : x12) {
			this->operands.first()->operands.last()->multiply(el);
		}
		for (auto& el : x21) {
			this->operands.first()->operands.last()->multiply(el);
		}

		for (auto& el : x12) {
			this->operands.last()->multiply(el);
		}
		for (auto& el : x22) {
			this->operands.last()->multiply(el);
		}
//		qDebug() << this->print();

//		// TODO: make better decisions based on b11 & b21
//		if (b12 & b22 & b11 & b21) {
//			/* v11   v21   n11 + n12 *
//			 * --- + --- = --------- *
//			 * v12   v22      n2     */
//			// TODO: negative values
//			int v11 = this->operands.first()->operands.first()->eval();
//			int v12 = this->operands.first()->operands.last()->eval();
//			int v21 = this->operands.last()->operands.first()->eval();
//			int v22 = this->operands.last()->operands.last()->eval();
//			int greatestd = gcd(v12, v22);
//			int n2 = abs(v12 * v22) / greatestd;
//			int n11 = v11 * v22 / greatestd;
//			int n12 = v21 * v12 / greatestd;

//			ExprTree* ttree = new ExprTree(*this);

//			this->op.s         = "/";
//			this->op.type      = TokenType::B;
//			this->op.assoc     = Assoc::LEFT;
//			this->op.prec      = 5; // raw assoc TODO: enum
//			this->op.rightPrec = 6;
//			this->op.nextPrec  = 5;

//			this->operands.clear();
//			this->operands.append(ttree);
//			this->operands.append(new ExprTree());
//			this->operands.first()->operands.first()->setValue(n11);
//			this->operands.first()->operands.last()->setValue(n12);
//			this->operands.last()->setValue(n2);
//			break;
//		}
//		if (b12 & b22 & b11) {
//			/*  v11   v21*x   n11*x + n12  *
//			 *  --- + ----- = -----------  *
//			 *  v12    v22         n2      */
//			QStringList x = this->operands.last()->operands.first()->var();
//			int v11 = this->operands.first()->operands.first()->eval();
//			int v12 = this->operands.first()->operands.last()->eval();
//			int v21 = this->operands.last()->operands.first()->eval();
//			int v22 = this->operands.last()->operands.last()->eval();
//			int greatestd = gcd(v12, v22);
//			int n2 = abs(v12 * v22) / greatestd;
//			int n12 = v11 * v22 / greatestd;
//			int n11 = v21 * v12 / greatestd;

//			ExprTree* ttree = new ExprTree(*this);

//			this->op.s         = "/";
//			this->op.type      = TokenType::B;
//			this->op.assoc     = Assoc::LEFT;
//			this->op.prec      = 5; // raw assoc TODO: enum
//			this->op.rightPrec = 6;
//			this->op.nextPrec  = 5;

//			this->operands.clear();
//			this->operands.append(ttree);
//			this->operands.append(new ExprTree());
//			this->operands.first()->operands.first()->setValue(x.first());
//			this->operands.first()->operands.first()->op.type = TokenType::x;
//			this->operands.first()->operands.first()->multiply(n11);
//			this->operands.first()->operands.last()->setValue(n12);
//			this->operands.last()->setValue(n2);
//			break;
//		}
//		if (b12 & b22 & b21) {
//			/*  v11*x   v21   n12*x + n11  *
//			 *  ----- + --- = -----------  *
//			 *  v12     v22        n2      */
//			QStringList x = this->operands.first()->operands.first()->var();
//			int v11 = this->operands.first()->operands.first()->eval();;
//			int v12 = this->operands.first()->operands.last()->eval();
//			int v21 = this->operands.last()->operands.first()->eval();
//			int v22 = this->operands.last()->operands.last()->eval();
//			int greatestd = gcd(v12, v22);
//			int n2 = abs(v12 * v22) / greatestd;
//			int n12 = v11 * v22 / greatestd;
//			int n11 = v21 * v12 / greatestd;

//			ExprTree* ttree = new ExprTree(*this);

//			this->op.s         = "/";
//			this->op.type      = TokenType::B;
//			this->op.assoc     = Assoc::LEFT;
//			this->op.prec      = 5; // raw assoc TODO: enum
//			this->op.rightPrec = 6;
//			this->op.nextPrec  = 5;

//			this->operands.clear();
//			this->operands.append(ttree);
//			this->operands.append(new ExprTree());
//			this->operands.first()->operands.first()->setValue(x.first());
//			this->operands.first()->operands.first()->op.type = TokenType::x;
//			this->operands.first()->operands.first()->multiply(n12);
//			this->operands.first()->operands.last()->setValue(n11);
//			this->operands.last()->setValue(n2);
//			break;
//		}
//		if (b12 & b22) {
//			/*  v11*x   v21*y   n12*x + n11*y  *
//			 *  ----- + ----- = -------------  *
//			 *   v12     v22          n2       */
//			QStringList x1 = this->operands.first()->operands.first()->var();
//			QStringList x2 = this->operands.last()->operands.first()->var();
//			int v11 = this->operands.first()->operands.first()->eval();
//			int v12 = this->operands.first()->operands.last()->eval();
//			int v21 = this->operands.last()->operands.first()->eval();
//			int v22 = this->operands.last()->operands.last()->eval();
//			int greatestd = gcd(v12, v22);
//			int n2 = abs(v12 * v22) / greatestd;
//			int n12 = v11 * v22 / greatestd;
//			int n11 = v21 * v12 / greatestd;

//			ExprTree* ttree = new ExprTree(*this);

//			this->op.s         = "/";
//			this->op.type      = TokenType::B;
//			this->op.assoc     = Assoc::LEFT;
//			this->op.prec      = 5; // raw assoc TODO: enum
//			this->op.rightPrec = 6;
//			this->op.nextPrec  = 5;

//			this->operands.clear();
//			this->operands.append(ttree);
//			this->operands.append(new ExprTree());
//			this->operands.first()->operands.first()->setValue(x1.first());
//			this->operands.first()->operands.first()->op.type = TokenType::x;
//			this->operands.first()->operands.first()->multiply(n12);
//			this->operands.first()->operands.last()->setValue(x2.first());
//			this->operands.first()->operands.last()->op.type = TokenType::x;
//			this->operands.first()->operands.last()->multiply(n11);
//			this->operands.last()->setValue(n2);
//			break;
//		}
		break;
	}
	case chash("%"): {
		ExprTree* ttree = new ExprTree(*this);

		this->op.s         = "/";
		this->op.type      = TokenType::B;
		this->op.assoc     = Assoc::LEFT;
		this->op.prec      = 5; // raw assoc TODO: enum
		this->op.rightPrec = 6;
		this->op.nextPrec  = 5;

		this->operands.clear();
		this->operands.append(ttree);
		this->operands.append(new ExprTree());
		this->operands.last()->setValue("1");
		break;
	}
	case chash("^"): {
		ExprTree* ttree = new ExprTree(*this);

		this->op.s         = "/";
		this->op.type      = TokenType::B;
		this->op.assoc     = Assoc::LEFT;
		this->op.prec      = 5; // raw assoc TODO: enum
		this->op.rightPrec = 6;
		this->op.nextPrec  = 5;

		this->operands.clear();
		this->operands.append(ttree);
		this->operands.append(new ExprTree());
		this->operands.last()->setValue("1");
		break;
	}
	default:
		Q_UNIMPLEMENTED();
		break;
	}
}

double
MufExprParser::ExprTree::eval()
{
	// TODO: improve
	if (op.type == TokenType::v) {
		return op.s.toDouble();
	}
	if (op.type == TokenType::x) {
		return 1; // 5x returns 5?
	}
	if (op.type == TokenType::U and
	    op.assoc == Assoc::PREFIX and
	    op.s == "-") {
		return -this->operands.first()->eval();
	}
	switch (chash(this->op.s)) {
	case chash("/"): {
		Q_UNIMPLEMENTED();
		return this->operands.first()->eval() /
		       this->operands.last()->eval();
		break;
	}
	case chash("*"): {
		return this->operands.first()->eval() *
		       this->operands.last()->eval();
		break;
	}
	case chash("+"): {
		return this->operands.first()->eval() +
		       this->operands.last()->eval();
		break;
	}
	case chash("-"): {
		return this->operands.first()->eval() -
		       this->operands.last()->eval();
		break;
	}
	case chash("%"): {
		Q_UNIMPLEMENTED();
		return (int)this->operands.first()->eval() %
		       (int)this->operands.last()->eval();
		break;
	}
	default:
		break;
	}
	qWarning("value check on non-value");
	return 0;
}

bool MufExprParser::ExprTree::isInt()
{
	Q_UNIMPLEMENTED();
	return false;
}

bool
MufExprParser::ExprTree::isOdd()
{
	if (this->isValue()) {
		const double v = this->eval();
		if (v == static_cast<int>(v)) {
			return static_cast<int>(v) % 2;
		}
	}
	qWarning("value check on non-value");
	return false;
}

bool MufExprParser::ExprTree::isEven()
{
	if (this->isValue()) {
		const double v = this->eval();
		if (v == static_cast<int>(v)) {
			return !static_cast<int>(v) % 2;
		}
	}
	qWarning("value check on non-value");
	return false;
}

QString
MufExprParser::ExprTree::print()
{
	QString t = op.s;
	if (operands.isEmpty()) {
		return t; // exit cond
	}
	t.append("(");
	t.append(operands.first()->print());
	for (int i = 1; i < operands.size(); ++i) {
		t.append(", ");
		t.append(operands.at(i)->print());
	}
	t.append(")");
	return t;
}

QString
MufExprParser::ExprTree::toLatex()
{
	QString t = "";
	switch (operands.size()) {
	case 0:
		switch (chash(op.s)) {
		case chash("("):
		case chash("["):
		case chash("{"):
			return "\\left(";
			break;
		case chash(")"):
		case chash("]"):
		case chash("}"):
			return "\\right)";
			break;
		case chash("inf"):
			return "{\\infty}";
			break;
		case chash("NaN"):
			return "{\\text{NaN}}";
			break;
		default:
			return "{" + op.s + "}";
			break;
		}
		Q_UNREACHABLE();
		break;
	case 1:
		switch (op.assoc) {
		case Assoc::PREFIX:
			return "{" + op.s +
			       "\\left(" + operands.first()->toLatex() +
			       "\\right)}";
			break;
		case Assoc::POSTFIX:
			return "{\\left(" + operands.first()->toLatex() +
			       "\\right)" + op.s + "}";
//			return operands.first()->toLatex() + op.s;
			break;
		default: // do nothing
			Q_UNREACHABLE();
			break;
		}
		Q_UNREACHABLE();
		break;
	case 2:
		switch (chash(op.s)) {
		case chash("/"):
			t.append("{\\frac{");
			t.append(operands.first()->toLatex());
			t.append("}{");
			t.append(operands.last()->toLatex());
			t.append("}}");
			return t;
			break;
		case chash("*"):
			t.append("{");
			t.append(operands.first()->toLatex());
			t.append("\\cdot");
			t.append(operands.last()->toLatex());
			t.append("}");
			return t;
			break;
		case chash("%"):
			t.append("{");
			t.append(operands.first()->toLatex());
			t.append("\\!\\!\\!\\!\\mod");
			t.append(operands.last()->toLatex());
			t.append("}");
			return t;
			break;
		default:
			t.append("{");
			t.append(operands.first()->toLatex());
			t.append(op.s);
			t.append(operands.last()->toLatex());
			t.append("}");
			return t;
			break;
		}
		Q_UNREACHABLE();
		break;
	default: // probably a function
		t.append("{");
		t.append(op.s);
		t.append("\\left(");
		t.append(operands.first()->toLatex());
		for (int i = 1; i < operands.size(); ++i) {
			t.append(", ");
			t.append(operands.at(i)->toLatex());
		}
		t.append("\\right)");
		t.append("}");
		return t;
		break;
	}
}

void
MufExprParser::ExprTree::reduce()
{
	if (op.type == TokenType::v or
	    op.type == TokenType::x or
	    operands.isEmpty()) { // no operands but not a value
		// nothing to do
		// tabun
		return;
	}
//	this->toFrac();
	switch (chash(op.s)) {
	case chash("/"): {
		bool v1 = this->operands.first()->isValue();
		bool v2 = this->operands.last()->isValue();
		if (v1 & v2 & numeric) {
			// compute
			int g = gcd(this->operands.first()->eval(),
			            this->operands.last()->eval());
			this->operands.first()->setValue(
			        this->operands.first()->eval() / g);
			this->operands.last()->setValue(
			        this->operands.last()->eval() / g);
			this->numeric = false;
			this->reduce();
			this->numeric = true;
			break;
		}
		if (v2 and this->operands.last()->eval() == 0) {
			// x/0 = inf
			this->setValue("inf"); //unreasonable?
			break;
		}
		if (v1 and this->operands.first()->eval() == 0) {
			// 0/x = 0
			this->setValue(0);
			break;
		}
		if (v2 and this->operands.last()->eval() == 1) {
			// x/1 = x
			this->setChild(0);
			break;
		}
		if (v1 and this->operands.first()->eval() == 1) {
			// 1/x = 1/x
			// do nothing
			break;
		}

		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		if (n1 <  0 and n2 <  0) {
			// -x * -y = x * y
			// -x / -y = x / y
			this->operands.first()->negate();
			this->operands.last()->negate();
		}
		break;
	}
	case chash("*"): {
		bool v1 = this->operands.first()->isValue();
		bool v2 = this->operands.last()->isValue();
		if (v1 & v2 & numeric) {
			// compute
			this->setValue(this->operands.first()->eval() *
			               this->operands.last()->eval());
			break;
		}
		if (v2 and this->operands.last()->eval() == 0) {
			// x*0 = 0
			this->setValue(0);
			break;
		}
		if (v1 and this->operands.first()->eval() == 0) {
			// 0*x = 0
			this->setValue(0);
			break;
		}
		if (v2 and this->operands.last()->eval() == 1) {
			// x*1 = x
			this->setChild(0);
			break;
		}
		if (v1 and this->operands.first()->eval() == 1) {
			// 1*x = x
			this->setChild(1);
			break;
		}

		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		if (n1 <  0 and n2 <  0) {
			// -x * -y = x * y
			// -x / -y = x / y
			this->operands.first()->negate();
			this->operands.last()->negate();
		}
		break;
	}
	case chash("-"): {
		bool v1 = this->operands.first()->isValue();
		bool v2 = this->operands.last()->isValue();
		if (v1 & v2 & numeric) {
			// compute
			this->setValue(this->operands.first()->eval() -
			               this->operands.last()->eval());
			break;
		}
		if (v2 and this->operands.last()->eval() == 0) {
			// x-0 = x
			this->setChild(0);
			break;
		}
		if (v1 and this->operands.first()->eval() == 0) {
			// 0-x = -x
			this->setChild(1);
			this->negate();
			break;
		}
		if (this->op.type == TokenType::U) {
			int n1 = this->operands.first()->negative();
			if (n1 < 0) {
				this->operands.first()->negate();
			}
		} else {
			int n1 = this->operands.first()->negative();
			int n2 = this->operands.last()->negative();
			if (n1 <  0 and n2 <  0) {
				// -x - -y = -x + y = y - x
				std::swap(this->operands.first(),
				          this->operands.last());
				this->operands.first()->negate();
				this->operands.last()->negate();
			}
			if (n1 <  0 and n2 >= 0) {
				// -x - y = -(x + y)
				op.s = "+";
				this->operands.first()->negate();
				this->prefixUnary();
			}
			if (n1 >= 0 and n2 <  0) {
				// x - -y = x + y
				op.s = "+";
				this->operands.last()->negate();
			}
			if (n1 >= 0 and n2 >= 0) {
				// x - y
				// do nothing
			}
		}
		break;
	}
	// -x + -y = -(x + y)
	// -x + y = y - x
	case chash("+"): {
		bool v1 = this->operands.first()->isValue();
		bool v2 = this->operands.last()->isValue();
		if (v1 & v2 & numeric) {
			// compute
			this->setValue(this->operands.first()->eval() +
			               this->operands.last()->eval());
			break;
		}
		if (v2 and this->operands.last()->eval() == 0) {
			// x+0 = x
			this->setChild(0);
			break;
		}
		if (v1 and this->operands.first()->eval() == 0) {
			// 0+x = x
			this->setChild(1);
			break;
		}

		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		if (n1 <  0 and n2 <  0) {
			// -x + -y = -(x + y)
			this->operands.first()->negate();
			this->operands.last()->negate();
			this->prefixUnary();
		}
		if (n1 <  0 and n2 >= 0) {
			// -x + y = y - x
			op.s = "-";
			this->operands.first()->negate();
			std::swap(this->operands.first(),
			          this->operands.last());
		}
		if (n1 >= 0 and n2 <  0) {
			// x + -y = x - y
			op.s = "-";
			this->operands.last()->negate();
		}
		if (n1 >= 0 and n2 >= 0) {
			// x + y
			// do nothing
		}
		break;
	}
	case chash("^"): {
		bool v1 = this->operands.first()->isValue();
		bool v2 = this->operands.last()->isValue();
		if (v1 & v2 & numeric) {
			// compute
			this->setValue(std::pow(this->operands.first()->eval(),
			                        this->operands.last()->eval()));
			break;
		}
		if (v1 and this->operands.first()->eval() == 0) {
			// 0^x = 0
			this->setValue("0");
			break;
		}
		if (v1 and this->operands.first()->eval() == 1) {
			// 1^x = 1
			this->setValue("1");
			break;
		}
		if (v2 and this->operands.last()->eval() == 0) {
			// x^0 = 1
			this->setValue(1);
			break;
		}
		if (v2 and this->operands.last()->eval() == 1) {
			// x^1 = x
			this->setChild(0);
			break;
		}

		int n = this->operands.first()->negative();
		if (n < 0) {
			if (this->operands.last()->isValue() and
			    this->operands.last()->isEven()) {
				this->operands.first()->negate();
			}
			if (this->operands.last()->isValue() and
			    this->operands.last()->isOdd()) {
				this->operands.first()->negate();
				this->negate();
//				traverse(this);
			}
		}
		break;
	}
	case chash("%"): {
		bool v1 = this->operands.first()->isValue();
		bool v2 = this->operands.last()->isValue();
		if (v1 & v2 & numeric) {
			// compute
			// TODO: fix type
			this->setValue((int)this->operands.first()->eval() %
			               (int)this->operands.last()->eval());
			break;
		}
		break;
	}
	default:
		break;
	}

	for (auto& el : operands) {
		el->reduce();
	}
}

void
MufExprParser::ExprTree::negate()
{
	// god i dont wanna think about this orz
	if (op.type == TokenType::v or
	    op.type == TokenType::x or
	    operands.isEmpty()) { // no operands but not a value
		this->prefixUnary();
		return;
	}

	switch (chash(this->op.s)) {
	case chash("-"): {
		if (this->op.type == TokenType::U) {
			// neg(-x) = x
			auto tmp = *this;
			*this = *tmp.operands.first();
//			delete tmp.operands.first();
			tmp.operands.removeFirst();
		} else {
			// neg(x - y) = y - x
			std::swap(this->operands.first(),
			          this->operands.last());

			int n1 = this->operands.first()->negative();
			int n2 = this->operands.last()->negative();
			if (n1 <  0 and n2 <  0) {
				// neg(-x - -y) = x - y
				this->operands.first()->negate();
				this->operands.last()->negate();
			}
			if (n1 <  0 and n2 >= 0) {
				// neg(-x - y) = x + y
				this->op.s = "+";
				this->operands.first()->negate();
			}
			if (n1 >= 0 and n2 <  0) {
				// neg(x - -y) = -(x + y)
				this->op.s = "+";
				this->operands.last()->negate();
				this->prefixUnary();
			}
			if (n1 >= 0 and n2 >= 0) {
				// neg(x - y) = y - x
				std::swap(this->operands.first(),
				          this->operands.last());
			}
			break;
		}
		break;
	}
	case chash("+"): {
		// neg(x + y) = -x + -y
		// neg(x + y) = -x - y
		// neg(x + y) = -y - x
		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		if (n1 <  0 and n2 <  0) {
			// neg(-x + -y) = x + y
			this->operands.first()->negate();
			this->operands.last()->negate();
		}
		if (n1 <  0 and n2 >= 0) {
			// neg(-x + y) = x - y
			op.s = "-";
			this->operands.first()->negate();
		}
		if (n1 >= 0 and n2 <  0) {
			// neg(x + -y) = y - x
			op.s = "-";
			std::swap(this->operands.first(),
			          this->operands.last());
			this->operands.first()->negate();
		}
		if (n1 >= 0 and n2 >= 0) {
			// neg(x + y) = -(x + y)
			this->prefixUnary();
		}
		break;
	}
	case chash("*"):
	case chash("/"): {
		int n1 = this->operands.first()->negative();
		int n2 = this->operands.last()->negative();
		if (n1 < n2) {
			this->operands.first()->negate();
		} else {
			this->operands.last()->negate();
		}
		break;
	}
	case chash("^"): {
		this->prefixUnary();
		break;
	}
	case chash("%"):
		// i dont wanna bother
		Q_UNIMPLEMENTED();
		return;
		break;
	default:
		Q_UNIMPLEMENTED();
		return;
		break;
	}
	return;
	//      Q_UNREACHABLE();
}

void
MufExprParser::ExprTree::multiply(const int& factor)
{
	ExprTree* ttree = new ExprTree(*this);

	this->op.s         = "*";
	this->op.type      = TokenType::B;
	this->op.assoc     = Assoc::LEFT;
	this->op.prec      = 5; // raw assoc TODO: enum
	this->op.rightPrec = 6;
	this->op.nextPrec  = 5;

	this->operands.clear();
	this->operands.append(new ExprTree);
	this->operands.first()->setValue(factor);
	this->operands.append(ttree);
}

void
MufExprParser::ExprTree::multiply(const QString& var)
{
	ExprTree* ttree = new ExprTree(*this);

	this->op.s         = "*";
	this->op.type      = TokenType::B;
	this->op.assoc     = Assoc::LEFT;
	this->op.prec      = 5; // raw assoc TODO: enum
	this->op.rightPrec = 6;
	this->op.nextPrec  = 5;

	this->operands.clear();
	this->operands.append(ttree);
	this->operands.append(new ExprTree);
	this->operands.last()->setValue(var);
	this->operands.last()->op.type = TokenType::x;
}

QStringList
MufExprParser::ExprTree::var()
{
	if (op.type == TokenType::v) {
		return {};
	}
	if (op.type == TokenType::x) {
		return {op.s}; // 5x returns 5?
	}
	QStringList r;
	for (auto& el : this->operands) {
		if (el->op.type == TokenType::x) {
			r.append(el->op.s);
		}
	}
	return r;
}

void
MufExprParser::ExprTree::prefixUnary()
{
	ExprTree* ttree = new ExprTree(*this);

	this->op.s         = "-";
	this->op.type      = TokenType::U;
	this->op.assoc     = Assoc::PREFIX;
	this->op.prec      = 6; // raw assoc TODO: enum
	this->op.rightPrec = 0;
	this->op.nextPrec  = 0;

	this->operands.clear();
	this->operands.append(ttree);
}

void
MufExprParser::ExprTree::setValue(const double& v)
{
	for (auto& el : this->operands) {
		delete el;
		el = nullptr;
	}
	this->operands.clear();
	this->op.s         = QString::number(std::abs(v));
	this->op.type      = TokenType::v;
	this->op.assoc     = Assoc::NONE;
	this->op.prec      = 0; // raw assoc TODO: enum
	this->op.rightPrec = 0;
	this->op.nextPrec  = 0;
	if (v < 0) {
		this->prefixUnary();
	}
}

void
MufExprParser::ExprTree::setValue(const QString& v)
{
	bool b = v.at(0) == '-';
	for (auto& el : this->operands) {
		delete el;
		el = nullptr;
	}
	this->operands.clear();
	if (b) {
		this->op.s = v.mid(1); // cut off -
	} else {
		this->op.s = v;
	}
	this->op.type      = TokenType::v;
	this->op.assoc     = Assoc::NONE;
	this->op.prec      = 0; // raw assoc TODO: enum
	this->op.rightPrec = 0;
	this->op.nextPrec  = 0;
	if (b) {
		this->prefixUnary();
	}
}

void
MufExprParser::ExprTree::setChild(const int& i)
{
	for (int j = this->operands.size() - 1; j >= 0; --j) {
		if (j == i) {
			continue;
		}
		delete this->operands.at(j);
		this->operands.removeAt(j);
	}
	auto tmp = *this;
	*this = *tmp.operands.first();
	tmp.operands.first()->operands.clear();
	delete tmp.operands.first();
	tmp.operands.clear();
}
