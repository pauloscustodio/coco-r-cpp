#include "Stdio.h"

#include "SymbolTable.h"

#include "Parser.h"
#include "Scanner.h"
#include <sys/timeb.h>
#include <wchar.h>

using namespace Taste;

int main (int argc, char *argv[]) {

	if (argc == 3) {
		wchar_t *fileName = coco_string_create(argv[1]);
		Taste::Scanner *scanner = new Taste::Scanner(fileName);
		Taste::Parser *parser = new Taste::Parser(scanner);
		parser->tab = new Taste::SymbolTable(parser);
		parser->gen = new Taste::CodeGenerator();
		parser->Parse();
		if (parser->errors->count == 0) {
			parser->gen->Decode();
			parser->gen->Interpret(argv[2]);
		}

		coco_string_delete(fileName);
		delete parser->gen;
		delete parser->tab;
		delete parser;
		delete scanner;
	} else
		printf("-- No source file specified\n");

	return 0;

}
