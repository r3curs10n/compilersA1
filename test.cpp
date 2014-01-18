#include <iostream>
#include <fstream>
#include "lexer.h"
using namespace std;
int main(){
	ifstream f("tst");
	lexer l(f);
	while (!l.eof){
		cout<<l.currentToken().value<<(int)l.currentToken().type<<endl;
		l.advance();
	}
	return 0;
}
