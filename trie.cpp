//ignore, redundant file (implementation in .h file)

#include "trie.h"

template<class T> trie<T>::trie (T defaultData): _defaultData(defaultData){
	reset();
}

template<class T> void trie<T>::reset () {
	fill (_stateData, _stateData+MAXSTATES, _defaultData);
	for (int i=0; i<MAXSTATES; i++){
		fill (_dfa[i], _dfa[i]+26, REJ);
	}
	_curState = BEGIN;
	_newState = 2;
}

template<class T> int trie<T>::newState() {
	newstate++;
	return _newState-1;
}

template<class T> void trie<T>::insert (string s, T tag) {
	int i=0;
	int state = BEGIN;
	while (i<s.length() && _dfa[state][s[i]-'a']!=-1){
		state = _dfa[state][s[i]-'a'];
		i++;
	}
	//add new states if required
	while (i<s.length()){
		_dfa[state][s[i]-'a'] = state = newState();
		i++;
	}
	_stateData[state] = tag;
}

template<class T> void trie<T>::refresh (){
	_curState = BEGIN;
}

template<class T> void trie<T>::input (char c) {
	_curState = _dfa[_curState][c-'a'];
}

template<class T> T trie<T>::getSD () {
	return _stateData[_curState];
}
