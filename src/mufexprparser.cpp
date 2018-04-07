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

	mOperators.push(op_sent);
	tree = nullptr;

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
			qDebug() << "tree:" << input;
//			qDebug() << tree->print();
//			tree->reduce();
//			qDebug() << "reduced tree:";
//			return tree->print();
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
		ExprTree* t = exprTD(5); // for "/[*/%]/"? idk
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
//		Q_UNIMPLEMENTED(); // push next
		pushOperator(next());
		consume();
		if (!pSY()) {
			qDebug() << "second p";
			return false;
		}
	}
	while (mOperators.top() != op_sent) {
		qDebug() << "this?";
//		Q_UNIMPLEMENTED(); // pop
		popOperator();
		qDebug() << "this?";
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
//		Q_UNIMPLEMENTED(); // push next
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

QString
MufExprParser::ExprTree::toLatex()
{
	QString t = "";
	switch (operands.size()) {
	case 0:
		if (op.s == "(" or
		    op.s == "[" or
		    op.s == "{") {
			return "\\left(";
		}
		if (op.s == ")" or
		    op.s == "]" or
		    op.s == "}") {
			return "\\right)";
		}
		return op.s;
		break;
	case 1:
		switch (op.assoc) {
		case Assoc::PREFIX:
			return op.s + operands.front()->toLatex();
			break;
		case Assoc::POSTFIX:
			return operands.front()->toLatex() + op.s;
			break;
		default: // do nothing
			Q_UNREACHABLE();
			break;
		}
		Q_UNREACHABLE();
		break;
	case 2:
		if (op.s == "/") {
			t.append("\\frac{");
			t.append(operands.front()->toLatex());
			t.append("}{");
			t.append(operands.back()->toLatex());
			t.append("}");
			return t;
		} else {
			t.append("{");
			t.append(operands.front()->toLatex());
			t.append(op.s);
			t.append(operands.back()->toLatex());
			t.append("}");
			return t;
		}
		Q_UNREACHABLE();
		break;
	default: // probably a function
		t.append("{");
		t.append(op.s);
		t.append("\\left(");
		t.append(operands.front()->toLatex());
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
//	if (left != nullptr) {
//		left->reduce();
//	}
//	if (right != nullptr) {
//		right->reduce();
//	}
//	if (op.type == TokenType::B) {
//		/*
//		if (left->op.type  == TokenType::U and
//		    left->op.assoc == Assoc::PREFIX and
//		    right->op.type  == TokenType::U and
//		    right->op.assoc == Assoc::PREFIX) {
//			ExprTree* t = left;
//			left = left->right;
//			t->right = nullptr;
//			delete t;
//			t = right;
//			right = right->right;
//			t->right = nullptr;
//			delete t;
//		} else */
//		if (left->op.type  == TokenType::v and
//		    right->op.type  == TokenType::v) {
//			if (op.s == "*") {
//				op = left->op;
//				op.s = left->value() * right->value();
//			} else if (op.s == "/") {
//				op = left->op;
//				op.s = left->value() / right->value();
//			} else if (op.s == "%") {
//				op = left->op;
//				op.s = (int)left->value() % (int)right->value();
//			} else if (op.s == "+") {
//				op = left->op;
//				op.s = left->value() + right->value();
//			} else if (op.s == "-") {
//				op = left->op;
//				op.s = left->value() - right->value();
//			}
//		}
//	} else if (op.type  == TokenType::U and
//	           op.assoc == Assoc::PREFIX and
//	           right->op.type  == TokenType::U and
//	           right->op.assoc == Assoc::PREFIX) {
//		ExprTree* t = right;
//		op = right->right->op;
//		left  = right->right->left;
//		right = right->right->right;
//		t->right->left  = nullptr;
//		t->right->right = nullptr;
//		delete t;
//	} else if (op.type  == TokenType::U and
//	           op.assoc == Assoc::PREFIX and
//	           right->op.type  == TokenType::B and
//	           right->op.s == "-") {
//		ExprTree* t = right;
//		op = right->op;
//		op.s = "+";
//		left  = right->left;
//		right = right->right;
//		t->left  = nullptr;
//		t->right = nullptr;
//		delete t;
//	}
}

