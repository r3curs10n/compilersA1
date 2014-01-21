#include "lexer.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
using namespace std;

lexer::token::token(lexer::TokenType t, string v, int l): type(t), value(v), lineNumber(l){
}

lexer::lexer (istream& _fs):_ls(_fs), _curToken(lexer::NA, "", 0), _keywords(lexer::NA){
	eof = false;
	
	_keywords.insert("if", lexer::IF);
	_keywords.insert("then", lexer::THEN);
	_keywords.insert("while", lexer::WHILE);
	_keywords.insert("do", lexer::DO);
	
	advance();
}

lexer::token lexer::currentToken(){
	return _curToken;
}

bool lexer::validToken(){
	return _curToken.type != lexer::NA;
}

bool lexer::matches(lexer::TokenType t){
	if (t == _curToken.type){
		advance();
		return true;
	}
	return false;
}

lexer::token lexer::at(int p){

	if (p==0) return _curToken;

	int offset=0;
	token t(NA, "", _curToken.lineNumber);
	
	for (int i=0; i<p; i++){
		int c;
		
		while ((c=_ls.at(offset))!=EOF && isspace((char)c)){
			if (c=='\n') t.lineNumber++;
			offset++;
		}
		
		if (c==EOF){
			t.type = lexer::EOI;
			t.value = "";
			continue;
		}
	
		if (_ls.match(":=", offset)){
			t.type = lexer::ASSIGN;
			t.value = ":=";
			offset += 2;
			continue;
		}
	
		if (_ls.match("+", offset)){
			t.type = lexer::PLUS;
			t.value = "+";
			offset += 1;
			continue;
		}
	
		if (_ls.match("(", offset)){
			t.type = lexer::LP;
			t.value = "(";
			offset += 1;
			continue;
		}
	
		if (_ls.match(")", offset)){
			t.type = lexer::RP;
			t.value = ")";
			offset += 1;
			continue;
		}
	
		if (_ls.match(";", offset)){
			t.type = lexer::SEMI;
			t.value = ";";
			offset += 1;
			continue;
		}
		
		if (c>='0' && c<='9'){
			//number
			stringstream s;
			while ((c=_ls.at(offset))>='0' && c<='9' && c!=EOF){
				s.put((char)c);
				offset++;
			}
			t.type = lexer::NUM;
			t.value = s.str();
			continue;
		}
	
		if (isalpha(c)){
			//id or keyword
			_keywords.refresh();
		
			stringstream s;
			while ((c=_ls.at(offset))!=EOF && isalpha(c)){
				s.put(c);
				_keywords.input(c);
				offset++;
			}
		
			//if token is a valid keyword
			if (_keywords.validState()){
				t.type = _keywords.getSD();
				t.value = s.str();
				continue;
			}
		
			//token is an id
			t.type = lexer::ID;
			t.value = s.str();
			continue;
		}
	
		t.type = lexer::NA;
		t.value = "";
		
	}
	
	return t;
}

void lexer::advance(){
	int c;
	while ((c=_ls.peek())!=EOF && isspace((char)c)){
		if (c=='\n') _curToken.lineNumber++;
		_ls.next();
	}
	
	//TODO: integrate symbols into trie based matching
	
	if (c==EOF){
		eof = true;
		_curToken.type = lexer::EOI;
		_curToken.value = "";
		return;
	}
	
	if (_ls.matchAndConsume(":=")){
		_curToken.type = lexer::ASSIGN;
		_curToken.value = ":=";
		return;
	}
	
	if (_ls.matchAndConsume("+")){
		_curToken.type = lexer::PLUS;
		_curToken.value = "+";
		return;
	}
	
	if (_ls.matchAndConsume("(")){
		_curToken.type = lexer::LP;
		_curToken.value = "(";
		return;
	}
	
	if (_ls.matchAndConsume(")")){
		_curToken.type = lexer::RP;
		_curToken.value = ")";
		return;
	}
	
	if (_ls.matchAndConsume(";")){
		_curToken.type = lexer::SEMI;
		_curToken.value = ";";
		return;
	}
	
	if (c>='0' && c<='9'){
		//number
		stringstream s;
		while ((c=_ls.peek())>='0' && c<='9' && c!=EOF){
			s.put((char)c);
			_ls.next();
		}
		_curToken.type = lexer::NUM;
		_curToken.value = s.str();
		return;
	}
	
	if (isalpha(c)){
		//id or keyword
		_keywords.refresh();
		stringstream s;
		while ((c=_ls.peek())!=EOF && isalpha(c)){
			s.put(c);
			_keywords.input(c);
			_ls.next();
		}
		
		//if token is a valid keyword
		if (_keywords.validState()){
			_curToken.type = _keywords.getSD();
			_curToken.value = s.str();
			return;
		}
		
		//token is an id
		_curToken.type = lexer::ID;
		_curToken.value = s.str();
		return;
	}
	
	_curToken.type = lexer::NA;
	_curToken.value = "";
}
