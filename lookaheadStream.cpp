#include "lookaheadStream.h"
#include <deque>
#include <istream>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;
bool lookaheadStream::exists (int p){
	if (p < this->buffer.size()){
		return true;
	}
	int c;
	while (p >= this->buffer.size()){
		c = this-> fs.get();
		if (c==EOF){
			this->eof = true;
			return false;
		}
		this->buffer.push_back(c);
	}
	return true;
}

int lookaheadStream::at (int p){
	if (!this->exists(p)){
		return EOF;
	}
	return this->buffer[p];
}

int lookaheadStream::peek (){
	return this->at(0);
}

int lookaheadStream::next(){
	if (this->exists(0)){
		int e = this->buffer.front();
		this->buffer.pop_front();
		return e;
	}
	return EOF;
}

bool lookaheadStream::match (string s){
	if (!this->exists(s.length()-1)) return false;
	for (int i=0; i<s.length(); i++){
		if (s[i]!=this->at(i)) return false;
	}
	return true;
}

bool lookaheadStream::matchAndConsume (string s){
	if (!this->match(s)) return false;
	for (int i=0; i<s.length(); i++)
		this->next();
	return true;
}

lookaheadStream::lookaheadStream (istream& _fs):fs(_fs), eof(false){
}
