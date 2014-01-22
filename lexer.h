#ifndef H_LEXER
#define H_LEXER
#include <iostream>
#include <istream>
#include <cstdio>
#include <string>
#include "lookaheadStream.h"
#include "trie.h"
using namespace std;
class lexer {

public:
	enum TokenType {
		SEMI, PLUS, MINUS, TIMES, DIVIDES, ASSIGN, EQ, GR, LS, LP, RP, NUM, ID, NA, EOI, IF, THEN, WHILE, DO, BEGIN, END
	};
	
	typedef struct token {
		TokenType type;
		string value;
		int lineNumber;
		
		token(TokenType, string, int);
		void set(TokenType, string);
	} token;

private:
	lookaheadStream _ls;
	token _curToken;
	trie<TokenType> _keywords;
	
public:
	token e;
	token currentToken();
	void advance();
	lexer(istream& _fs);
	bool eof;
	bool validToken();
	token at(int);
	bool matches(TokenType);
};
#endif
