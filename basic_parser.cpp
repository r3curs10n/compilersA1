#include <iostream>
#include "lexer.h"
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <map>
#include <set>
#include "name.c"
using namespace std;

typedef struct expri {
	lexer::TokenType op;
	string val; //makes sense only if token is of type num
	char* r;
	
	expri (lexer::TokenType _op, string _val, char* _r): op(_op), val(_val), r(_r){
	}
	
	expri (){
	}
} expi;

void statements();
void statement();
expri term();
expri expr();
expri expr_math();
void opt_statements();
expri factor();

map<string, char*> ids;
set<char*> pinned;
lexer* l;

int indent=0;

void fenter(string msg){
	//indent++;
	//for (int i=0; i<indent; i++) cout<<"  ";
	//cout<<"enter "<<msg<<endl;
}

void fexit(string msg){
	//for (int i=0; i<indent; i++) cout<<"  ";
	//cout<<"exit "<<msg<<endl;
	//indent--;
}

string newlabel(){
	static int c=-1;
	stringstream s;
	c++;
	s<<"lbl"<<c;
	return s.str();
}

int main(){
	ifstream f("tst");
	l = new lexer(f);
	
	statements();
	cout<<"OK!"<<endl;
	//cout<<string(ids["idf"]);
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
		string x = l->e.value;
		if (l->matches(lexer::ASSIGN)){
			expri r = expr();
			if (!pinned.count(r.r)) freename(r.r);
			
			if (!ids.count(x)){
				//assign new register to variable
				ids[x] = newname();
				pinned.insert(ids[x]);
			}
			char * idn = ids[x];
			if (idn!=r.r) printf("mov %%%s, %%%s\n", idn, r.r);
		}
	} else if (l->matches(lexer::IF)){
		expri r = expr();
		string lbl = newlabel();
		printf("test %%%s, %%%s\n", r.r, r.r);
		printf("jz %s\n", lbl.c_str());
		freename(r.r);
		if (l->matches(lexer::THEN)){
			statement();
			printf("%s: nop\n", lbl.c_str());
		} else {
			cout<<"error"<<endl;
			exit(0);
		}
	} else if (l->matches(lexer::WHILE)){
		string lblLoop = newlabel();
		string lblExit = newlabel();
		printf("%s: nop\n", lblLoop.c_str());
		expri r = expr();
		printf("test %%%s, %%%s\n", r.r, r.r);
		printf("jz %s\n", lblExit.c_str());
		freename(r.r);
		if (l->matches(lexer::DO)){
			statement();
			printf("jmp %s\n", lblLoop.c_str());
			printf("%s: nop\n", lblExit.c_str());
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

expri expr(){
	fenter("expr");
	expri r = expr_math();
	if (l->matches(lexer::LS)){
		expri rr = expr_math();
		printf("cmp %%%s, %%%s\n", r.r, rr.r);
		printf("mov %%%s, $1\n", r.r);
		string lbl = newlabel();
		printf("jl %s\n", lbl.c_str());
		printf("mov %%%s, $0", r.r);
		printf("%s: nop\n", lbl.c_str());
		freename(rr.r);
	} else if (l->matches(lexer::EQ)){
		expri rr = expr_math();
		printf("cmp %%%s, %%%s\n", r.r, rr.r);
		printf("mov %%%s, $1\n", r.r);
		string lbl = newlabel();
		printf("je %s\n", lbl.c_str());
		printf("mov %%%s, $0", r.r);
		printf("%s: nop\n", lbl.c_str());
		freename(rr.r);
	} else if (l->matches(lexer::GR)){
		expri rr = expr_math();
		printf("cmp %%%s, %%%s\n", r.r, rr.r);
		printf("mov %%%s, $1\n", r.r);
		string lbl = newlabel();
		printf("jg %s\n", lbl.c_str());
		printf("mov %%%s, $0", r.r);
		printf("%s: nop\n", lbl.c_str());
		freename(rr.r);
	} else {
		//simple math expression
	}
	fexit("expr");
	return r;
}

expri expr_math(){
	
	fenter("expr_math");
	
	expri r = term();
	while (true){
		if (l->matches(lexer::PLUS)){
			expri rr = term();
			printf("add %%%s, %%%s\n", r.r, rr.r);
			freename(rr.r);
		} else if (l->matches(lexer::MINUS)){
			expri rr = term();
			printf("sub %%%s, %%%s\n", r.r, rr.r);
			freename(rr.r);
		} else {
			break;
		}
	}
	fexit("expr_math");
	return r;
}

expri term(){
	fenter("term");
	
	expri r=factor();
	
	while (true){
		if (l->matches(lexer::TIMES)){
			expri rr = factor();
			printf("mul %%%s, %%%s\n", r.r, rr.r);
			freename(rr.r);
		} else if (l->matches(lexer::DIVIDES)){
			//some work needed here
			//empty rdx:rax pair for division
			expri rr = factor();
			printf("div %%%s, %%%s\n", r.r, rr.r);
			freename(rr.r);
		} else {
			break;
		}
	}
	
	fexit("term");
	return r;
}

expri factor(){
	fenter("factor");
	expri r;
	if (l->matches(lexer::ID)){
		//
		r.r = newname();
		printf("mov %%%s, %%%s\n",r.r, ids[l->e.value]);
	} else if (l->matches(lexer::NUM)){
		//
		r.r = newname();
		printf("mov %%%s, $%s\n", r.r, l->e.value.c_str());
	} else if (l->matches(lexer::LP)){
		r = expr_math();
		if (l->matches(lexer::RP)){
			//
		} else {
			cout<<"error"<<endl;
			exit(0);
		}
	} else {
		cout<<"error"<<endl;
		exit(0);
	}
	
	fexit("factor");
	return r;
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
