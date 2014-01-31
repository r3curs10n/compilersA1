#ifndef H_CODEGEN
#define H_CODEGEN

#include <iostream>
#include <vector>
#include <map>
using namespace std;

typedef int obj;

const static char* _regs[] = {"rax", "rbx", "rcx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
	const static int _regsS = sizeof(_regs)/sizeof(char*);

typedef struct objInfo {
	int addr;
	int reg;
	bool inuse;
} objInfo;

class codeGen {

private:
	vector<objInfo> _objs;
	vector<bool> _allocator;
	int _allocp;
	int _maxObjs;
	int _tempStart;
	map<string, int> _ids;
	
	
	int _regUsedBy[_regsS];
	int _unusedRegs;
	int _pinnedReg;
	
	int newaddr();
	void freeaddr(int);
	
	int newreg(obj);
	void freereg(obj);
	
	void evictReg(int);
	
	void integ(string);
	
public:
	codeGen(vector<string> ids, int maxObjs);
	obj newobj();
	void freeobj(obj);
	obj getObjById(string id);
	string newlabel();

	void flushIds();

	void xADD(obj x, obj y);
	void xSUB(obj x, obj y);
	void xMUL(obj x, obj y);
	void xMOV(obj x, obj y);
	void xMOV(string x, obj y);
	
	void xDIV(obj x, obj y);
	void xTEST(obj x, obj y);
	void xNOP(){};
	void xJMP(string lbl);
	void xJZ(string lbl);
	void xCMP(obj x, obj y);
	void xCMOVL(obj x, obj y){}; //unused
	void xCMOVL(string x, obj y);
	void xCMOVE(obj x, obj y){};//unused
	void xCMOVE(string x, obj y);
	void xCMOVG(obj x, obj y){};//unused
	void xCMOVG(string x, obj y);
	void xLABEL(string lbl);
};

#endif
