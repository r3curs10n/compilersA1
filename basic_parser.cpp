#include <iostream>
#include "lexer.h"
#include <fstream>
#include <cstdlib>
using namespace std;

void statements();
void statement();
void expression();
void expressionp();
void term();

lexer* l;

int main(){
	ifstream f("tst");
	l = new lexer(f);
	
	statements();
	cout<<"OK!"<<endl;
}

void statements(){
	while (l->currentToken().type!=lexer::EOI){
		statement();
		if (l->currentToken().type!=lexer::SEMI){
			cout<<"ERROR on line "<<l->currentToken().lineNumber<<endl;
			exit(0);
		}
		l->advance();
	}
}

void statement(){
	expression();
}

void expression(){
	term();
	expressionp();
}

void expressionp(){
	if (l->currentToken().type != lexer::PLUS){
		return;
	}
	l->advance();
	term();
	expressionp();
}

void term(){
	if (l->currentToken().type == lexer::LP){
		l->advance();
		expression();
		if (l->currentToken().type != lexer::RP){
			cout<<"ERROR"<<endl;
			exit(0);
		}
		l->advance();
		return;
	}
	if (l->currentToken().type == lexer::NUM || l->currentToken().type == lexer::ID){
		l->advance();
		return;
	}
	cout<<"ERROR"<<endl;
	exit(0);
}
