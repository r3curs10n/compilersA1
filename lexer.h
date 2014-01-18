#include <iostream>
#include <istream>
#include <cstdio>
#include <string>
#include "lookaheadStream.h"
using namespace std;
class lexer {

public:
	enum TokenType {
		SEMI, PLUS, MINUS, TIMES, DIVIDE, ASSIGN, EQ, GR, LS, LP, RP, NUM, ID, NA
	};
	
	typedef struct token {
		TokenType type;
		string value;
		int lineNumber;
		
		token(TokenType, string, int);
		
	} token;

private:
	lookaheadStream _ls;
	token _curToken;
	
public:
	token currentToken();
	void advance();
	lexer(istream& _fs);
	bool eof;
	bool validToken();
};
