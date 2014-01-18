#include "lexer.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
using namespace std;

lexer::token::token(lexer::TokenType t, string v, int l){
	this->type = t;
	this->value = v;
	this->lineNumber = l;
}

lexer::lexer (istream& _fs):_ls(_fs), _curToken(lexer::NA, "", 0){
	this->eof = false;
	this->advance();
}

lexer::token lexer::currentToken(){
	return this->_curToken;
}

bool lexer::validToken(){
	return this->_curToken.type != lexer::NA;
}

void lexer::advance(){
	int c;
	
	while ((c=this->_ls.peek())!=EOF && isspace((char)c)){
		if (c=='\n') this->_curToken.lineNumber++;
		this->_ls.next();
	}
	
	//TODO: try to use a better matching process
	
	if (c==EOF){
		this->eof = true;
		return;
	}
	
	if (this->_ls.matchAndConsume(":=")){
		this->_curToken.type = lexer::ASSIGN;
		this->_curToken.value = ":=";
		return;
	}
	
	if (this->_ls.matchAndConsume("+")){
		this->_curToken.type = lexer::PLUS;
		this->_curToken.value = "+";
		return;
	}
	
	if (this->_ls.matchAndConsume("(")){
		this->_curToken.type = lexer::LP;
		this->_curToken.value = "(";
		return;
	}
	
	if (this->_ls.matchAndConsume(")")){
		this->_curToken.type = lexer::RP;
		this->_curToken.value = ")";
		return;
	}
	
	if (this->_ls.matchAndConsume(";")){
		this->_curToken.type = lexer::SEMI;
		this->_curToken.value = ";";
		return;
	}
	
	if (c>='0' && c<='9'){
		//number
		stringstream s;
		while ((c=this->_ls.peek())>='0' && c<='9'){
			s.put((char)c);
		}
		this->_curToken.type = lexer::NUM;
		this->_curToken.value = s.str();
		return;
	}
	
	if (isalpha(c)){
		//id
		stringstream s;
		while ((c=this->_ls.peek())!=EOF && isalpha(c)){
			s.put(c);
			this->_ls.next();
		}
		this->_curToken.type = lexer::ID;
		this->_curToken.value = s.str();
		return;
	}
	
	this->_curToken.type = lexer::NA;
	this->_curToken.value = "";
}
