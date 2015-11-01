/*----------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than
Coco/R itself) does not fall under the GNU General Public License.
-----------------------------------------------------------------------*/

#include "Generator.h"
#include "Scanner.h"

namespace Coco {

	Generator::Generator(Tab *tab, Errors *errors) {
		this->errors = errors;
		this->tab = tab;
		fram = NULL;
		gen = NULL;
		frameFile = NULL;
	}

	FILE* Generator::OpenFrame(const char* frame) {
		if (coco_string_length(tab->frameDir) != 0) {
			frameFile = coco_string_create_append(tab->frameDir, "/");
			coco_string_merge(frameFile, frame);
			char *chFrameFile = coco_string_create(frameFile);
			fram = fopen(chFrameFile, "r");
			delete [] chFrameFile;
		}
		if (fram == NULL) {
			delete [] frameFile;
			frameFile = coco_string_create_append(tab->srcDir, frame);  /* pdt */
			char *chFrameFile = coco_string_create(frameFile);
			fram = fopen(chFrameFile, "r");
			delete [] chFrameFile;
		}
		if (fram == NULL) {
			char *message = coco_string_create_append("-- Cannot find : ", frame);
			errors->Exception(message);
			delete [] message;
		}

	    return fram;
	}


	FILE* Generator::OpenGen(const char *genName) { /* pdt */
		char *fn = coco_string_create_append(tab->outDir, genName); /* pdt */
		char *chFn = coco_string_create(fn);

		if ((gen = fopen(chFn, "r")) != NULL) {
			fclose(gen);
			char *oldName = coco_string_create_append(fn, ".old");
			char *chOldName = coco_string_create(oldName);
			remove(chOldName); rename(chFn, chOldName); // copy with overwrite
			coco_string_delete(chOldName);
			coco_string_delete(oldName);
		}
		if ((gen = fopen(chFn, "w")) == NULL) {
			char *message = coco_string_create_append("-- Cannot generate : ", genName);
			errors->Exception(message);
			delete [] message;
		}
		coco_string_delete(chFn);
		coco_string_delete(fn);

		return gen;
	}


	void Generator::GenCopyright() {
		FILE *file = NULL;

		if (coco_string_length(tab->frameDir) != 0) {
			char *copyFr = coco_string_create_append(tab->frameDir, "/Copyright.frame");
			char *chCopyFr = coco_string_create(copyFr);
			file = fopen(chCopyFr, "r");
			delete [] copyFr;
			delete [] chCopyFr;
		}
		if (file == NULL) {
			char *copyFr = coco_string_create_append(tab->srcDir, "Copyright.frame");
			char *chCopyFr = coco_string_create(copyFr);
			file = fopen(chCopyFr, "r");
			delete [] copyFr;
			delete [] chCopyFr;
		}
		if (file == NULL) {
			return;
		}

		FILE *scannerFram = fram;
		fram = file;

		CopyFramePart(NULL);
		fram = scannerFram;

		fclose(file);
	}

	void Generator::GenPrefixFromNamespace() {
		const char *nsName = tab->nsName;
		if (nsName == NULL || coco_string_length(nsName) == 0) {
			return;
		}
		const int len = coco_string_length(nsName);
		int startPos = 0;
		do {
			int curLen = coco_string_indexof(nsName + startPos, COCO_CPP_NAMESPACE_SEPARATOR);
			if (curLen == -1) { curLen = len - startPos; }
			char *curNs = coco_string_create(nsName, startPos, curLen);
			fprintf(gen, "%s_", curNs);
			coco_string_delete(curNs);
			startPos = startPos + curLen + 1;
		} while (startPos < len);
	}

	void Generator::SkipFramePart(const char *stop) {
		CopyFramePart(stop, false);
	}

	void Generator::CopyFramePart(const char *stop) {
		CopyFramePart(stop, true);
	}

	void Generator::CopyFramePart(const char* stop, bool generateOutput) {
		char startCh = 0;
		int endOfStopString = 0;
		char ch = 0;

		if (stop != NULL) {
			startCh = stop[0];
			endOfStopString = coco_string_length(stop)-1;
		}

		fscanf(fram, "%c", &ch); //	fram.ReadByte();
		while (!feof(fram)) { // ch != EOF
			if (stop != NULL && ch == startCh) {
				int i = 0;
				do {
					if (i == endOfStopString) return; // stop[0..i] found
					fscanf(fram, "%c", &ch); i++;
				} while (ch == stop[i]);
				// stop[0..i-1] found; continue with last read character
				if (generateOutput) {
					char *subStop = coco_string_create(stop, 0, i);
					fprintf(gen, "%s", subStop);
					coco_string_delete(subStop);
				}
			} else {
				if (generateOutput) { fprintf(gen, "%c", ch); }
				fscanf(fram, "%c", &ch);
			}
		}
		if (stop != NULL) {
			char *message = coco_string_create_append(" -- Incomplete or corrupt frame file: ", frameFile);
			errors->Exception(message);
			delete [] message;
		}
	}

}