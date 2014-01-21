#include <iostream>
#include "lexer.h"
#include <fstream>
#include <cstdlib>
using namespace std;

void statements();
void statement();
void term();
void expr();
void expr_math();
void opt_statements();
void aterm();
lexer* l;

int indent=0;

void fenter(string msg){
	indent++;
	for (int i=0; i<indent; i++) cout<<"  ";
	cout<<"enter "<<msg<<endl;
}

void fexit(string msg){
	for (int i=0; i<indent; i++) cout<<"  ";
	cout<<"exit "<<msg<<endl;
	indent--;
}

int main(){
	ifstream f("tst");
	l = new lexer(f);
	
	statements();
	cout<<"OK!"<<endl;
}

void statements(){
	fenter("statements");
	if (!l->matches(lexer::EOI) && !(l->currentToken().type==lexer::END)){
		statement();
		if (l->matches(lexer::SEMI)){
			statements();
		}
		else {
			cout<<"errorStatements"<<endl;
			exit(0);
		}
	}
	fexit("statements");
}

void statement(){
	fenter("statement");
	if (l->matches(lexer::ID)){
		if (l->matches(lexer::ASSIGN)){
			expr();
		}
	} else if (l->matches(lexer::IF)){
		expr();
		if (l->matches(lexer::THEN)){
			statement();
		} else {
			cout<<"error"<<endl;
			exit(0);
		}
	} else if (l->matches(lexer::WHILE)){
		expr();
		if (l->matches(lexer::DO)){
			statement();
		} else {
			cout<<"error"<<endl;
			exit(0);
		}
	} else if (l->matches(lexer::BEGIN)){
		opt_statements();
		if (l->matches(lexer::END)){
			//nothing
		} else {
			cout<<"error"<<endl;
			exit(0);
		}
	}
	fexit("statement");
}

void opt_statements(){
	fenter("opt_satement");
	if (l->at(0).type == lexer::END){
		return;
	}
	statements();
	fexit("opt_statement");
}

void expr(){
	fenter("expr");
	expr_math();
	if (l->matches(lexer::LS)){
		expr_math();
	} else if (l->matches(lexer::EQ)){
		expr_math();
	} else if (l->matches(lexer::GR)){
		expr_math();
	} else {
		//simple math expression
	}
	fexit("expr");
}

void expr_math(){
	fenter("expr_math");
	aterm();
	if (l->matches(lexer::PLUS)){
		expr_math();
	} else if (l->matches(lexer::MINUS)){
		expr_math();
	}
	fexit("expr_math");
}

void aterm(){
	fenter("aterm");
	
	//term is necessary
	if (l->matches(lexer::NUM)){
		//plain number
		term();
	} else if (l->matches(lexer::ID)){
		//variable
		term();
	} else if (l->matches(lexer::TIMES)){
		term();
	} else if (l->matches(lexer::DIVIDES)){
		term();
	} else if (l->matches(lexer::LP)){
		expr_math();
		if (l->matches(lexer::RP)){
			term();
		} else {
			cout<<"errorTerm"<<endl;
			exit(0);
		}
	} else {
		cout<<"error"<<endl;
		exit(0);
	}
	
	fexit("aterm");
}

void term(){
	fenter("term");
	if (l->matches(lexer::NUM)){
		//plain number
		term();
	} else if (l->matches(lexer::ID)){
		//variable
		term();
	} else if (l->matches(lexer::TIMES)){
		aterm();
	} else if (l->matches(lexer::DIVIDES)){
		aterm();
	} else if (l->matches(lexer::LP)){
		expr_math();
		if (l->matches(lexer::RP)){
			term();
		} else {
			cout<<"errorTerm"<<endl;
			exit(0);
		}
	}
	fexit("term");
}

/*void statements(){
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
	if (l->matches(lexer::PLUS){
		term();
		expressionp();
	}
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
}*/
