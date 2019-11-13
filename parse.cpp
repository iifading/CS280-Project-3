/**
 * Ricky Palaguachi
 * CS280
 * Program 3
 * parse.cpp
 */

#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

namespace Parser {
    bool pushed_back = false;
    Lex	pushed_token;

    static Lex GetNextToken(istream& in, int& line) {
        if( pushed_back ) {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }

    static void PushBackToken(Lex& t) {
        if (pushed_back) {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }
}

static int error_count = 0;

void ParseError(int line, string msg) {
	++error_count;
	cout << line << ": " << msg << endl;
}

// Prog := Slist
ParseTree *Prog(istream& in, int& line) {
    //Declare a pointer to object of rule meant to be returned
	ParseTree *sl = Slist(in, line);
    
    //Reject a null pointer
	if( sl == 0 )
		ParseError(line, "No statements in program");
    //Reject any syntax errors 
	if( error_count )
		return 0;

    //Return pointer to Slist
	return sl; 
}

// Slist := SC { Slist } | Stmt SC { Slist }
ParseTree *Slist(istream& in, int& line) {
	Lex t = Parser::GetNextToken(in, line);
	
	if (t == SC){
		while ( true ) {
			ParseTree *sl = Slist(in, line);
			if (sl == 0) {
				return new StmtList(0, 0);
			}
		}
	}

	Parser::PushBackToken(t);

	ParseTree *s = Stmt(in, line);
	
	if( s == 0 )
		return 0;
	
	t = Parser::GetNextToken(in, line);

	if (t != SC){
		ParseError(line, "missing semicolon");
		return 0;
	}
	

	return new StmtList(s, Slist(in,line));
}

// Stmt := IfStmt | PrintStmt | LetStmt | LoopStmt
ParseTree *Stmt(istream& in, int& line) {
    Lex peekToken = Parser::GetNextToken(in, line);
    ParseTree *stmt = 0;

    if (peekToken == IF) { //IfStmt
        stmt = IfStmt(in, line);
    }
    else if (peekToken == LET) { //LetStmt
        stmt = LetStmt(in, line);
    }
    else if (peekToken == PRINT) { //PrintStmt
        stmt = PrintStmt(in, line);
    } 
    else if (peekToken == LOOP) { //LoopStmt
        stmt = LoopStmt(in, line);
    }

	ParseError(line, "invalid expression");
    return stmt; 
}

// IfStmt := IF Expr BEGIN Slist END
ParseTree *IfStmt(istream& in, int& line) {
    //already verified IF token
    ParseTree *expr = Expr(in, line);
    if (expr == 0){
        ParseError(line, "Invalid Expr");
		return 0;
    }

	Lex t = Parser::GetNextToken(in, line);
    if (t != BEGIN){
		Parser::PushBackToken(t);
        ParseError(line, "Missing token BEGIN");
		return 0;
    }

    ParseTree *sl = Slist(in, line);
    if (sl == 0) {
        ParseError(line, "Invalid Slist");
		return 0;
    }
	
	t = Parser::GetNextToken(in, line);
    if (t != END){
        ParseError(line, "Missing token END");
    }

    return new If(line, expr, sl);
}
// LetStmt := LET ID Expr
ParseTree *LetStmt(istream& in, int& line) {
    //already verified LET token
	Lex id = Parser::GetNextToken(in, line);

	if (id != ID){
		ParseError(line, "Identifier error");
		Parser::PushBackToken(id);
		return 0;
	}

	ParseTree * expr = Expr(in, line);
	
	if (expr == 0){
		ParseError(line, "invalid assignment expression");
		return 0;
	}

	return new Let(id, expr);
}

// PrintStmt := PRINT Expr
ParseTree *PrintStmt(istream& in, int& line) {
    //already verified PRINT token
    ParseTree *expr = Expr(in, line);
	if (expr == 0){
		ParseError(line, "invalid expression");
		return 0;
	}

    return new Print(line, expr);
}

// LoopStmt := LOOP Expr BEGIN Slist END
ParseTree *LoopStmt(istream& in, int& line) {
    //already verified LOOP token
	ParseTree * expr = Expr(in, line);

	if (expr == 0){
		ParseError(line, "invalid expr");
		return 0;
	}
	Lex t = Parser::GetNextToken(in, line);

	if (t != BEGIN){
		Parser::PushBackToken(t);
		ParseError(line, "Missing BEGIN token for LOOP");
		return 0;
	}

	ParseTree * sl = Slist(in, line);

	if (sl == 0) {
		ParseError(line, "error in loop body");
		return 0;
	}

	t = Parser::GetNextToken(in, line);

	if (t != END) {
		Parser::PushBackToken(t);
		ParseError(line, "Missing END token for LOOP");
		return 0;
	}

	return new Loop(line, expr, sl);
}

// Expr := Prod { (PLUS | MINUS) Prod}
ParseTree *Expr(istream& in, int& line) {
	ParseTree *t1 = Prod(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Lex t = Parser::GetNextToken(in, line);

		if( t != PLUS && t != MINUS ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing Expr after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

// Prod := Rev { (STAR | SLASH) Rev}
ParseTree *Prod(istream& in, int& line) {
    ParseTree *t1 = Rev(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Lex t = Parser::GetNextToken(in, line);

		if( t != STAR && t != SLASH ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing Expr after operator");
			return 0;
		}

		if( t == STAR )
			t1 = new TimesExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new DivideExpr(t.GetLinenum(), t1, t2);
	}//while
}

// Rev := BANG Rev | Primary
ParseTree *Rev(istream& in, int& line) {
	Lex t = Parser::GetNextToken(in, line);
	
	if (t == BANG) {
		ParseTree * expr = Expr(in, line);
		return new BangExpr(line, expr);
	}

	Parser::PushBackToken(t);
	ParseTree * p = Primary(in, line);
	return p;
}

// Primary := ID | INT | STR | LPAREN Expr RPAREN
ParseTree *Primary(istream& in, int& line) {
	Lex t = Parser::GetNextToken(in, line);

	if( t == ID ) {
		return new Ident(t);
	}
	else if( t == INT ) {
		return new IConst(t);
	}
	else if( t == STR ) {
		return new SConst(t);
	}
	else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(line, "Missing expression after (");
			return 0;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return 0;
	}

	ParseError(line, "Primary expected");
	return 0;
}

