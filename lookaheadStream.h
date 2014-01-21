#ifndef H_LAS
#define H_LAS
#include <deque>
#include <istream>
#include <string>
using namespace std;
class lookaheadStream{
private:
	istream& fs;
	deque<char> buffer;
	
public:
	bool eof;
	
	lookaheadStream(istream& _fs);
	bool exists(int p);
	int at(int p);
	int peek();
	int next();
	
	bool match(string s);
	bool match(string s, int offset);
	bool matchAndConsume(string s);
};
#endif
