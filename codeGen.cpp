#include "codeGen.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cassert>
#include <cstdlib>
using namespace std;

//integrity check
void codeGen::integ(string s){
	//cout<<s<<endl;
	/*for (int i=0; i<_maxObjs; i++){
		if (_objs[i].reg!=-1){
			//printf(">>>>>>>>>>>>>>>>%d %d\n", i, _regUsedBy[_objs[i].reg]);
			assert(i == _regUsedBy[_objs[i].reg]);
		}
	}*/
}

codeGen::codeGen (vector<string> ids, int maxObjs){
	_maxObjs = maxObjs;
	_objs.resize(_maxObjs);
	_allocator.resize(_maxObjs);
	fill (_regUsedBy, _regUsedBy+_regsS, -1);
	for (int i=0; i<_regsS; i++){
		_regUsedBy[i]=-1;
		//cout<<"->"<<_regUsedBy[i]<<endl;
	}
	//cout<<_regsS<<endl;
	_unusedRegs = _regsS;
	_pinnedReg = -1;
	
	_allocp=1;
	
	for (int i=0; i<_allocator.size(); i++){
		_allocator[i] = false;
	}
	for (int i=0; i<_objs.size(); i++){
		_objs[i].addr=0;
		_objs[i].reg=-1;
		_objs[i].inuse=false;
	}
	
	int i=0;
	for (; i<ids.size(); i++){
		obj x = _ids[ids[i]] = newobj();
		if (_objs[x].addr==0) _objs[x].addr = newaddr();
	}
	integ("constructor");
}

int codeGen::newaddr(){
	int p = _allocp;
	while (true){
		if (_allocator[p]==false){
			_allocator[p] = true;
			return p;
		}
		p++;
		if (p>=_allocator.size()) p=1;
	}
	integ("newaddr");
}

void codeGen::freeaddr(obj o){
	_allocator[_objs[o].addr] = false;
	_objs[o].addr=0;
	if (_objs[o].addr==_allocp){
		_allocp--;
		if (_allocp<=1) _allocp=1;
	}
	integ("freeaddr");
}

int codeGen::getObjById (string id){
	if (_ids.count(id)==0){
		cout<<"ERROR: Unknown identifier"<<endl;
		exit(0);
	}
	return _ids[id];
}

string codeGen::newlabel (){
	static int c=0;
	stringstream s;
	s<<"lbl"<<c;
	c++;
	return s.str();
}

int codeGen::newreg (obj o){
	assert(o>=0);
	static int c=0;
	int cc=c;
	while (_unusedRegs>0){
		if (_regUsedBy[c]==-1){
			_regUsedBy[c] = o;
			_objs[o].reg = c;
			_unusedRegs--;
			integ("newreg[");
			return c;
		}
		c++;
		if (c==_regsS) c=0;
		if (c==cc) break;
	}
	if (c==_pinnedReg) c=(c+1)%_regsS;
	
	if (_objs[_regUsedBy[c]].addr==0) _objs[_regUsedBy[c]].addr = newaddr();
	printf("movq %%%s, -%d(%%rbp)\n", _regs[c], _objs[_regUsedBy[c]].addr*8);
	_objs[_regUsedBy[c]].reg=-1;
	_regUsedBy[c] = o;
	_objs[o].reg = c;
	integ("newreg");
	return c;
}

void codeGen::freereg(obj o){
	if (_objs[o].reg==-1) return;
	_regUsedBy[_objs[o].reg] = -1;
	_objs[o].reg=-1;
	_unusedRegs++;
	integ("freereg");
	return;
}

void codeGen::evictReg(int r){
	if (_regUsedBy[r]==-1) return;
	obj o = _regUsedBy[r];
	if (_objs[o].addr==0) _objs[o].addr = newaddr();
	printf("movq %%%s, -%d(%%rbp)\n", _regs[_objs[o].reg], _objs[o].addr*8);
}

obj codeGen::newobj() {
	static int c=0;
	while (true){
		if (!_objs[c].inuse){
			_objs[c].inuse=true;
			break;
		}
		c++;
		if (c>=_maxObjs) c=0;
	}
	
	if (_unusedRegs>0){
		newreg(c);
		//cout<<"lkjdsf"<<endl;
	} else {
		_objs[c].addr = newaddr();
	}
	integ("newobj");
	return c;	
}

void codeGen::freeobj(obj o){
	_objs[o].inuse=false;
	freereg(o);
	freeaddr(o);
	integ("freeobj");
}

void codeGen::xADD(obj x, obj y){
	if (_objs[x].reg!=-1 && _objs[y].reg!=-1){
		printf("addq %%%s, %%%s\n", _regs[_objs[x].reg], _regs[_objs[y].reg]);
	} else if (_objs[x].reg!=-1){
		printf("addq %%%s, -%d(%%rbp)\n", _regs[_objs[x].reg], _objs[y].addr*8);
	} else if (_objs[y].reg!=-1){
		printf("addq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[y].reg]);
	} else {
		newreg(x);
		printf("movq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[x].reg]);
		printf("addq %%%s, -%d(%%rbp)\n", _regs[_objs[x].reg], _objs[y].addr*8);
	}
}

void codeGen::xSUB(obj x, obj y){
	if (_objs[x].reg!=-1 && _objs[y].reg!=-1){
		printf("subq %%%s, %%%s\n", _regs[_objs[x].reg], _regs[_objs[y].reg]);
	} else if (_objs[x].reg!=-1){
		printf("subq %%%s, -%d(%%rbp)\n", _regs[_objs[x].reg], _objs[y].addr*8);
	} else if (_objs[y].reg!=-1){
		printf("subq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[y].reg]);
	} else {
		newreg(x);
		printf("movq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[x].reg]);
		printf("subq %%%s, -%d(%%rbp)\n", _regs[_objs[x].reg], _objs[y].addr*8);
	}
}

void codeGen::xMUL(obj x, obj y){
	
	/*int rx, ry;
	if (_objs[x].reg==-1) newreg(x);
	rx = _objs[x].reg;
	_pinnedReg = rx;
	if (_objs[y].reg==-1) newreg(y);
	rx = _objs[y].reg;
	
	printf("imulq %%%s, %%%s\n", _regs[rx], _regs[ry]);
	
	_pinnedReg = -1;*/
	
	
	_pinnedReg = _objs[y].reg;
	if (_objs[x].reg==-1) {
		newreg(x);
		printf("movq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[x].reg]);
	}
	if (_objs[y].reg!=-1){
		printf("movq %%%s, %%rax\n", _regs[_objs[y].reg]);
		printf("imulq %%%s\n", _regs[_objs[x].reg]);
		printf("movq %%rax, %%%s\n", _regs[_objs[y].reg]);
	} else {
		printf("movq -%d(%%rbp), %%rax\n", _objs[y].addr*8);
		printf("imulq %%%s\n", _regs[_objs[x].reg]);
		printf("movq %%rax, -%d(%%rbp)\n", _objs[y].addr*8);
	}
	_pinnedReg=-1;
}

void codeGen::xDIV(obj x, obj y){
	vector<objInfo>::iterator oy = _objs.begin() + y;
	if (oy->reg!=0) {
		//evict RAX
		evictReg(0);
		if (oy->reg==-1){
			printf("movq -%d(%%rbp), %%rax\n", oy->addr*8);
		} else {
			printf("movq %%%s, %%rax\n", _regs[oy->reg]);
			freereg(y);
		}
		oy->reg = 0;
		_regUsedBy[0] = y;
	}
	
	_pinnedReg = 0;
	if (_objs[x].reg==-1) newreg(x);
	_pinnedReg = -1;
	
	printf("xorq %%rdx, %%rdx\n");
	printf("idivq %%%s\n", _regs[_objs[x].reg]);
}

void codeGen::xMOV(obj x, obj y){
	if (_objs[x].reg!=-1 && _objs[y].reg!=-1){
		printf("movq %%%s, %%%s\n", _regs[_objs[x].reg], _regs[_objs[y].reg]);
	} else if (_objs[y].reg==-1){
		printf("movq %%%s, -%d(%%rbp)\n", _regs[_objs[x].reg], _objs[y].addr*8);
	} else if (_objs[x].reg==-1){
		printf("movq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[y].reg]);
	} else {
		newreg(x);
		printf("movq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[x].reg]);
		printf("movq %%%s, -%d(%%rbp)\n", _regs[_objs[x].reg], _objs[y].addr * 8);
	}
}

void codeGen::xMOV(string s, obj y){
	if (_objs[y].reg!=-1){
		printf("movq %s, %%%s\n", s.c_str(), _regs[_objs[y].reg]);
	} else {
		printf("movq %s, -%d(%%rbp)\n", s.c_str(), _objs[y].addr * 8);
	}
}

void codeGen::xTEST(obj x, obj y){
	if (x!=y) cout<<"fuck... we dont care\n";
	if (_objs[x].reg==-1){
		newreg(x);
		printf("movq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[x].reg]);
	}
	printf("testq %%%s, %%%s\n", _regs[_objs[x].reg], _regs[_objs[x].reg]);
}

void codeGen::xJMP(string lbl){
	printf("jmp %s\n", lbl.c_str());
}

void codeGen::xJZ(string lbl){
	printf("jz %s\n", lbl.c_str());
}

void codeGen::xCMP(obj x, obj y){
	if (_objs[x].reg!=-1 && _objs[y].reg!=-1){
		printf("cmpq %%%s, %%%s\n", _regs[_objs[x].reg], _regs[_objs[y].reg]);
	} else if (_objs[x].reg!=-1){
		printf("cmpq -%d(%%rbp), %%%s\n", _objs[x].addr*8, _regs[_objs[y].reg]);
	} else if (_objs[y].reg!=-1){
		printf("cmpq %%%s, -%d(%%rbp)\n", _regs[_objs[x].reg], _objs[y].addr*8);
	} else {
		printf("cmpq -%d(%%rbp), -%d(%%rbp)\n", _objs[x].addr*8, _objs[y].addr*8);
	}
}

void codeGen::xCMOVL(string x, obj y){
	if (_objs[y].reg==-1) {
		newreg(y);
		printf("movq -%d(%%rbp), %%%s\n", _objs[y].addr*8, _regs[_objs[y].reg]);
	}
	printf("cmovl %s, %%%s\n", x.c_str(), _regs[_objs[y].reg]);
}

void codeGen::xCMOVE(string x, obj y){
	if (_objs[y].reg==-1) {
		newreg(y);
		printf("movq -%d(%%rbp), %%%s\n", _objs[y].addr*8, _regs[_objs[y].reg]);
	}
	printf("cmove %s, %%%s\n", x.c_str(), _regs[_objs[y].reg]);
}

void codeGen::xCMOVG(string x, obj y){
	if (_objs[y].reg==-1) {
		newreg(y);
		printf("movq -%d(%%rbp), %%%s\n", _objs[y].addr*8, _regs[_objs[y].reg]);
	}
	printf("cmovg %s, %%%s\n", x.c_str(), _regs[_objs[y].reg]);
}

void codeGen::xLABEL(string lbl){
	printf("%s: nop\n", lbl.c_str());
}

void codeGen::flushIds() {
	printf("flushIds: nop\n");
	for (map<string, int>::iterator i = _ids.begin(); i!=_ids.end(); i++){
		if (_objs[i->second].reg==-1) continue;
		printf("movq %%%s, -%d(%%rbp)\n", _regs[_objs[i->second].reg], _objs[i->second].addr * 8);
	}
}
