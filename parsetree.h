/**
 * Ricky Palaguachi
 * CS280
 * Program 3
 * parsetree.cpp
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
#include <string>
#include "lex.h"
using std::vector;
using std::map;
using std::string;


// NodeType represents all possible types
enum NodeType { 
    ERRTYPE, INTTYPE, STRTYPE 
};

// a "forward declaration" for a class to hold values
class Value;

class ParseTree {
    protected:
        int			linenum;
        ParseTree	*left;
        ParseTree	*right;

    public:
        ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
            : linenum(linenum), left(l), right(r) {}

        virtual ~ParseTree() {
            delete left;
            delete right;
        }

        int GetLineNumber() const { return linenum; }

        int MaxDepth() const {
            int lCount = 0;
            int rCount = 0;
            if(left != 0){
                lCount += left->MaxDepth();
            }
            if(right != 0){
                rCount += right->MaxDepth();
            }
            if(lCount > rCount){
                return lCount;
            }

            return rCount;
        }

        virtual NodeType  getNodeType() const { return ERRTYPE; }
        virtual int GetIntValue() const { return -1; }
        virtual bool IsIdent() const { return false; }
        virtual bool IsVar() const { return false; }
        virtual string GetStringValue() const { return ""; }
        virtual int IsBang() const { return 0; }
        virtual bool IsLet() const { return false; }
        ParseTree * getLeft() const { return left; }
        ParseTree * getRight() const { return right; }

        
        int BangCount() const {
            return 1;
        }

        virtual void CheckLetBeforeUse(map<std::string,bool>& var) {
        }
};



// := SC { Slist } | Stmt SC { Slist }
class StmtList : public ParseTree {

public:
    StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
};

/***** Statements *****/

// := LET ID Expr
class Let : public ParseTree {
	string id;

public:
	Let(Lex& t, ParseTree *expr) : ParseTree(t.GetLinenum(), expr), id(t.GetLexeme()) {}

	string GetId() const { return id; }
	bool IsLet() const { return true; }
};

// := PRINT Expr
class Print : public ParseTree {

public:
    Print(int& line, ParseTree *expr) : ParseTree(line, expr) {}
};

class Loop : public ParseTree {

public:
    Loop(int& line, ParseTree *expr, ParseTree *slist) : ParseTree(line, expr, slist) {}
};

//:= IF Expr BEGIN Slist END
class If : public ParseTree {

public:
    If(int& line, ParseTree *expr, ParseTree *slist) : ParseTree(line, expr, slist) {}
};

/***** Expressions *****/

// := Prod { PLUS Prod }
class PlusExpr : public ParseTree {
    NodeType nodeType;
    int intValue;
    string strValue;
public:

	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {
        /*
        //handle addition
        if ((l->getNodeType() == INTTYPE) && (r->getNodeType() == INTTYPE)){
            nodeType = INTTYPE;
            intValue = l->GetIntValue() + r->GetIntValue();
        }
        //handle string concatenation
        else if ((l->getNodeType() == STRTYPE) && (r->getNodeType() == STRTYPE)) {
            nodeType = STRTYPE;
            strValue = l->GetStringValue() + r->GetStringValue();
        }
        //invalid operand types
        else
            nodeType = ERRTYPE;
        */
    }

    NodeType getNodeType() const { return nodeType; }

    int GetIntValue() const {
        return intValue;
    }

    string GetStringValue() const {
        return strValue;
    }
};

// := Prod { MINUS Prod }
class MinusExpr : public ParseTree {
    NodeType nodeType;
    int value;
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {
        /*
        //handle subtraction
        if ((l->getNodeType() == INTTYPE) && (r->getNodeType() == INTTYPE)) {
            nodeType = INTTYPE;
            value = l->GetIntValue() - r->GetIntValue();
        }
        //handle error
        else {
            nodeType = ERRTYPE;
        }
        */
    }

    NodeType getNodeType() const { return nodeType; }

    int GetIntValue() const {
        return value;
    }
};

// := Rev { STAR Rev }
class TimesExpr : public ParseTree {
    NodeType nodeType;
    int intValue;
    string strValue;
public:
    TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line, l, r) {
        /*
        //handle integer multiplication
        if ((l->getNodeType() == INTTYPE) && (r->getNodeType() == INTTYPE)){
            nodeType = INTTYPE;
            intValue = l->GetIntValue() * (r->GetIntValue());
        }
        //handle string multiplication
        else if ( (l->getNodeType() == INTTYPE) && (r->getNodeType() == STRTYPE) ) {
            if (l->GetIntValue() < 0){
                nodeType = ERRTYPE;
                return;
            }
            
            nodeType = STRTYPE;
            strValue = "";
            for (int i = 0; i < l->GetIntValue(); i++){
                strValue += r->GetStringValue();
            }
        }
        else if ((l->getNodeType() == STRTYPE) && (r->getNodeType() == INTTYPE) ) {
            if (r->GetIntValue() < 0){
                nodeType = ERRTYPE;
                return;
            }

            nodeType = STRTYPE;
            strValue = "";
            for (int i = 0; i < r->GetIntValue(); i++){
                strValue += l->GetStringValue();
            }
        }
        else {
            nodeType = ERRTYPE;
        }
        */
    }//TimesExpr() constructor

    NodeType getNodeType() const { return nodeType; }
    int GetIntValue() const {
        return intValue;
    }
    
    string GetStringValue() const {
        return strValue;
    }
};

// := Rev { SLASH Rev }
class DivideExpr : public ParseTree {
    NodeType nodeType;
    int intValue;
    string strValue;
public:
    DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line, l, r) {
        /*
        //handle integer division
        if ((l->getNodeType() == INTTYPE) && (r->getNodeType() == INTTYPE)){
            nodeType = INTTYPE;
            intValue = l->GetIntValue() / r->GetIntValue();
        }
        else {
            nodeType = ERRTYPE;
        }
        */
    }//TimesExpr() constructor

    NodeType getNodeType() const { return nodeType; }
    int GetIntValue() const {
        return intValue; 
    }
    
    string GetStringValue() const {
        return strValue;
    }

    bool isBang() const { return true; }
};

// := BANG Rev
class BangExpr : public ParseTree {
    NodeType nodeType;
    int intVal;
    string strVal;

public: 
    BangExpr(int& line, ParseTree *expr) : ParseTree (line, expr){
        /*
        //handle reversing digits
        if (expr->getNodeType() == INTTYPE){
            nodeType = INTTYPE;
            intVal = std::stoi(reverse(std::to_string(expr->GetIntValue())));
        }
        //handle reversing strings
        else if (expr->getNodeType() ==STRTYPE){
            nodeType = STRTYPE;
            strVal = reverse(expr->GetStringValue());
        }
        else {
            nodeType = ERRTYPE;
        }
        */
    }

    //Reverse a string
    string reverse(string str){
        int n = str.length(); 
  
        // Swap character starting from two 
        // corners 
        for (int i = 0; i < n / 2; i++) {
            char temp = str[i];
            str[i] = str[n - i - 1];
            str[n - i - 1] = temp;
        }

        return str;
    }

    NodeType getNodeType() const { return nodeType; }
    int GetIntValue() const {
        return intVal;
    }
    
    string GetStringValue() const {
        return strVal;
    }

};

/***** Primary *****/

class IConst : public ParseTree {
	int val;

public:
	IConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}

    NodeType getNodeType() const { return INTTYPE; }
    int GetIntValue() const { return val; }
};


class SConst : public ParseTree {
	string val;

public:
	SConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}
    NodeType getNodeType() const { return STRTYPE; }
    string GetStringValue() const { return val; }
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Lex& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {}

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
};

#endif /* PARSETREE_H_ */
