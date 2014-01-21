#ifndef H_TRIE
#define H_TRIE
#include <iostream>
#include <string>
using namespace std;

#define MAXSTATES 5000
#define TRIE_BEGIN 1
#define TRIE_REJ 0

template <class T> class trie {

private:
	int _dfa[MAXSTATES][26];
	T _stateData[MAXSTATES];
	T _defaultData;
	int _curState;
	int _newState;
	
	int newState();
	
public:
	trie (T);
	void insert (string, T);
	void refresh ();
	void reset ();
	void input (char);
	T getSD ();
	bool validState();
};

//implementation is in the header file to handle templates

template<class T> trie<T>::trie (T defaultData): _defaultData(defaultData){
	reset();
}

template<class T> void trie<T>::reset () {
	fill (_stateData, _stateData+MAXSTATES, _defaultData);
	for (int i=0; i<MAXSTATES; i++){
		fill (_dfa[i], _dfa[i]+26, TRIE_REJ);
	}
	_curState = TRIE_BEGIN;
	_newState = 2;
}

template<class T> int trie<T>::newState() {
	_newState++;
	return _newState-1;
}

template<class T> void trie<T>::insert (string s, T tag) {
	int i=0;
	int state = TRIE_BEGIN;
	while (i<s.length() && _dfa[state][s[i]-'a']!=TRIE_REJ){
		state = _dfa[state][s[i]-'a'];
		i++;
	}
	//add new states if required
	while (i<s.length()){
		_dfa[state][s[i]-'a'] = newState();
		state = _dfa[state][s[i]-'a'];
		i++;
	}
	_stateData[state] = tag;
}

template<class T> void trie<T>::refresh (){
	_curState = TRIE_BEGIN;
}

template<class T> void trie<T>::input (char c) {
	_curState = _dfa[_curState][c-'a'];
}

template<class T> T trie<T>::getSD () {
	return _stateData[_curState];
}

template<class T> bool trie<T>::validState () {
	return getSD() != _defaultData;
}
#endif
