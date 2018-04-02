#ifndef MUFEXPRPARSER_H
#define MUFEXPRPARSER_H

#include <QObject>
#include <QList>
#include <QRegularExpression>
#include <QStack>
#include <QQueue>

class MufExprParser : public QObject
{
	Q_OBJECT
public:
	enum class Assoc {
		NONE,
		LEFT,
		RIGHT,
		PREFIX,
		POSTFIX
	};
	Q_ENUM(Assoc)
	enum class TokenType {
		P,
		B, // binary op
		E, // expression
		b, // brace
		U, // unary
		v, // value
		x, // variable
		NONE
	};
	Q_ENUM(TokenType)
	struct str_tok_t {
		QString         s;
		Assoc           assoc;
		int             prec;
		int             rightPrec;
		int             nextPrec;
		TokenType       type;
		friend bool operator>(const str_tok_t& lhs, const str_tok_t& rhs);
		friend bool operator<(const str_tok_t& lhs, const str_tok_t& rhs);
		friend bool operator>=(const str_tok_t& lhs, const str_tok_t& rhs);
		friend bool operator<=(const str_tok_t& lhs, const str_tok_t& rhs);
		friend bool operator==(const str_tok_t& lhs, const str_tok_t& rhs);
		friend bool operator!=(const str_tok_t& lhs, const str_tok_t& rhs);
	};
	struct pat_t {
		QString         str;
		TokenType       type;
		Assoc           assoc;
		int             prec;
		int             rightPrec;
		int             nextPrec;
		QRegularExpression re;
	};

	class ExprTree
	{
	public:
		ExprTree()
		        : left(nullptr), right(nullptr) {}
		ExprTree(const ExprTree& rhs)
		{
			this->op = rhs.op;
			this->left  = nullptr;
			this->right = nullptr;
			if (rhs.left != nullptr) {
				this->left = new ExprTree(*rhs.left);
			}
			if (rhs.right != nullptr) {
				this->right = new ExprTree(*rhs.right);
			}
		}
		ExprTree(str_tok_t v) // set value as operator
		        : left(nullptr), right(nullptr)
		{
			op = v;
		}
		ExprTree(str_tok_t _op, ExprTree _operand)
		{
			if (_op.assoc == Assoc::PREFIX) {
				op = _op;
				left = nullptr;
				right = new ExprTree(_operand);
			} else if (_op.assoc == Assoc::POSTFIX) {
				op = _op;
				left = new ExprTree(_operand);
				right = nullptr;
			} else {
				qDebug("ExprTree construct error");
//				qDebug() << _op.s << _op.type;
			}
		}
		ExprTree(str_tok_t _op, ExprTree* _operand)
		{
			if (_op.assoc == Assoc::PREFIX) {
				op = _op;
				left = nullptr;
				right = _operand;
			} else if (_op.assoc == Assoc::POSTFIX) {
				op = _op;
				left = _operand;
				right = nullptr;
			} else {
				qDebug("ExprTree construct error");
//				qDebug() << _op.s << _op.type;
			}
		}
		ExprTree(str_tok_t _op, ExprTree _left, ExprTree _right)
		        : left(new ExprTree(_left)), right(new ExprTree(_right))
		{
			op = _op;
		}
		ExprTree(str_tok_t _op, ExprTree* _left, ExprTree* _right)
		        : left(_left), right(_right)
		{
			op = _op;
		}

		~ExprTree()
		{
			op = op_sent;
			if (left != nullptr) {
				delete left;
			}
			if (right != nullptr) {
				delete right;
			}
		}

		double value()
		{
			if (op.type == TokenType::v) {
				return op.s.toDouble();
			} else {
				return 0;
			}
		}

		QString print()
		{
			QString t = op.s;
			if (op.type == TokenType::B) {
				t.append("(");
				t.append(left->print());
				t.append(", ");
				t.append(right->print());
				t.append(")");
			} else if (op.type  == TokenType::U and
			           op.assoc == Assoc::POSTFIX) {
				t.append("(");
				t.append(left->print());
				t.append(")");
			} else if (op.type  == TokenType::U and
			           op.assoc == Assoc::PREFIX) {
				t.append("(");
				t.append(right->print());
				t.append(")");
			}
			return t;
		}

		void reduce();

		str_tok_t       op;    // can be any operator or value
		ExprTree*       left;  // can be null
		ExprTree*       right; // can be null
	};

public:
	explicit MufExprParser(QObject* parent = nullptr);
	~MufExprParser()
	{
		if (tree != nullptr) {
			delete tree;
		}
	}

private:
	int             init();

	bool            expect(const QString& tok_s); // true if next.s == tok_s
	bool            expect(const str_tok_t& tok); // true if next == tok
	bool            expect(const TokenType& tok_t); // true if next.type == tok_t
	void            consume(); // removes next token
	str_tok_t       next();    // returns next token

	bool            tokenize(QString input);
	pat_t*          match(
	        QString s,
	        QList<pat_t> p,
	        str_tok_t* t,
	        QString* e);

	ExprTree*       exprTD(int p);
	ExprTree*       pTD();
	void            pushOperator(const str_tok_t& op);
	void            popOperator();

	ExprTree*       exprRD(int p);
	ExprTree*       pRD();
	bool            exprSY();
	bool            pSY();
	bool            exprR();
	bool            pR();


signals:

public slots:

	bool            exprRecognise(QString input);
	QString         exprParseSY(QString input);
	QString         exprParseRD(QString input);
	QString         exprParseTD(QString input);

private:
	static str_tok_t tok_end; // end token
	static str_tok_t op_sent; // sentinel
	pat_t           pat_eos; // end of string pattern
	QList<pat_t>    pat_ops; // list of operator patterns
	QList<pat_t>    pat_arg; // list of arg? patterns

	QQueue<str_tok_t> tokens; // tokens to parse
	QStack<str_tok_t> mOperators;
	QStack<ExprTree> mOperands;
//	QStack<str_tok_t> stack;

public:
//	QQueue<str_tok_t> queue;
	ExprTree*       tree;

	friend bool operator>(const str_tok_t& lhs, const str_tok_t& rhs);
};

#endif // MUFEXPRPARSER_H
