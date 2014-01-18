#include <iostream>
#include <fstream>
//#include "lexer.h"
#include "trie.h"
#include <string>
using namespace std;
int main(){
	/*ifstream f("tst");
	lexer l(f);
	while (!l.eof){
		cout<<l.currentToken().value<<(int)l.currentToken().type<<endl;
		l.advance();
	}*/
	
	trie<int> t(-1);
	t.insert(string("hello"),0);
	t.insert(string("heldo"),1);
	t.insert(string("aaaaa"),2);
	
	string s = "hehllo";
	
	for (int i=0; i<s.length(); i++){
		t.input(s[i]);
	}
	
	cout<<t.getSD()<<endl;
	
	return 0;
}
