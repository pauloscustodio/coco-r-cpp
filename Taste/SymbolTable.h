#if !defined(TASTE_SYMBOLTABLE_H__)
#define TASTE_SYMBOLTABLE_H__

#include "Scanner.h"

namespace Taste {

class Parser;
class Errors;

class Obj {  // object describing a declared name
public:
	char* name;		// name of the object
	int type;		// type of the object (undef for proc)
	Obj	*next;		// to next object in same scope
	int kind;		// var, proc, scope
	int adr;		// address in memory or start of proc
	int level;		// nesting level; 0=global, 1=local
	Obj *locals;		// scopes: to locally declared objects
	int nextAdr;	// scopes: next free address in this scope

	Obj() {
		name    = NULL;
		type    = 0;
		next    = NULL;
		kind    = 0;
		adr     = 0;
		level   = 0;
		locals  = NULL;
		nextAdr = 0;
	}

	~Obj() {
		coco_string_delete(name);
	}


};

class SymbolTable
{
public:
	const int // types
		undef, integer, boolean;

	const int // object kinds
		var, proc, scope;


	int curLevel;	// nesting level of current scope
	Obj *undefObj;	// object node for erroneous symbols
	Obj *topScope;	// topmost procedure scope

	Errors *errors;

	SymbolTable(Parser *parser);
	SymbolTable& operator=(const SymbolTable&);	// cannot be copied

	void Err(const char* msg);

	// open a new scope and make it the current scope (topScope)
	void OpenScope ();

	// close the current scope
	void CloseScope ();

	// create a new object node in the current scope
	Obj* NewObj (char* name, int kind, int type);

	// search the name in all open scopes and return its object node
	Obj* Find (char* name);

};

}; // namespace

#endif // !defined(TASTE_SYMBOLTABLE_H__)
