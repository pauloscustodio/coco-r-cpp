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
#include <sstream>
#include <string>
#include "Generator.h"
#include "Scanner.h"

namespace Coco {

	Generator::Generator(Tab *tab, std::shared_ptr<Errors> errors) {
		this->errors = errors;
		this->tab = tab;
		fram = NULL;
		gen = NULL;
	}

	FILE* Generator::OpenFrame(const std::wstring frame) {
		if (CocoUtil::coco_string_length(tab->frameDir) != 0) {
			frameFile = CocoUtil::coco_string_create_append(tab->frameDir, L"/");
			CocoUtil::coco_string_merge(frameFile, frame);
			std::string chFrameFile = CocoUtil::coco_string_create_char(frameFile);
			fram = fopen(chFrameFile.c_str(), "r");
		}
		if (fram == NULL) {
			frameFile = CocoUtil::coco_string_create_append(tab->srcDir, frame);  /* pdt */
			std::string chFrameFile = CocoUtil::coco_string_create_char(frameFile);
			fram = fopen(chFrameFile.c_str(), "r");
		}
		if (fram == NULL) {
			std::wstring message = CocoUtil::coco_string_create_append(L"-- Cannot find : ", frame);
			errors->Exception(message);
		}

	    return fram;
	}


	FILE* Generator::OpenGen(const std::wstring genName) { /* pdt */
		std::wstring fn = CocoUtil::coco_string_create_append(tab->outDir, genName); /* pdt */
		std::string chFn = CocoUtil::coco_string_create_char(fn);

		if ((gen = fopen(chFn.c_str(), "r")) != NULL) {
			fclose(gen);
			std::wstring oldName = CocoUtil::coco_string_create_append(fn, L".old");
			std::string chOldName = CocoUtil::coco_string_create_char(oldName);
			remove(chOldName.c_str()); rename(chFn.c_str(), chOldName.c_str()); // copy with overwrite
			CocoUtil::coco_string_delete(chOldName);
			CocoUtil::coco_string_delete(oldName);
		}
		if ((gen = fopen(chFn.c_str(), "w")) == NULL) {
			std::wstring message = CocoUtil::coco_string_create_append(L"-- Cannot generate : ", genName);
			errors->Exception(message);
		}
		CocoUtil::coco_string_delete(chFn);
		CocoUtil::coco_string_delete(fn);

		return gen;
	}


	void Generator::GenCopyright() {
		FILE *file = NULL;

		if (CocoUtil::coco_string_length(tab->frameDir) != 0) {
			std::wstring copyFr = CocoUtil::coco_string_create_append(tab->frameDir, L"/Copyright.frame");
			std::string chCopyFr = CocoUtil::coco_string_create_char(copyFr);
			file = fopen(chCopyFr.c_str(), "r");
		}
		if (file == NULL) {
			std::wstring copyFr = CocoUtil::coco_string_create_append(tab->srcDir, L"Copyright.frame");
			std::string chCopyFr = CocoUtil::coco_string_create_char(copyFr);
			file = fopen(chCopyFr.c_str(), "r");
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
		const std::wstring nsName = tab->nsName;
		if (CocoUtil::coco_string_length(nsName) == 0) {
			return;
		}
		const int len = CocoUtil::coco_string_length(nsName);
		int startPos = 0;
		do {
			int curLen = CocoUtil::coco_string_indexof(nsName.substr(startPos), COCO_CPP_NAMESPACE_SEPARATOR);
			if (curLen == -1) { curLen = len - startPos; }
			std::wstring curNs = CocoUtil::coco_string_create(nsName, startPos, curLen);
			fwprintf(gen, L"%ls_", curNs.c_str());
			CocoUtil::coco_string_delete(curNs);
			startPos = startPos + curLen + 1;
		} while (startPos < len);
	}

	void Generator::SkipFramePart(const std::wstring stop) {
		CopyFramePart(stop, false);
	}

	void Generator::CopyFramePart(const std::wstring stop) {
		CopyFramePart(stop, true);
	}

	void Generator::CopyFramePart(const std::wstring stop, bool generateOutput) {
		wchar_t startCh = 0;
		int endOfStopString = 0;
		wchar_t ch = 0;

		if (stop.length() != 0) {
			startCh = stop[0];
			endOfStopString = CocoUtil::coco_string_length(stop)-1;
		}

		fwscanf(fram, L"%lc", &ch); //	fram.ReadByte();
		while (!feof(fram)) { // ch != EOF
			if (stop.length() == 0 && ch == startCh) {
				int i = 0;
				do {
					if (i == endOfStopString) return; // stop[0..i] found
					fwscanf(fram, L"%lc", &ch); i++;
				} while (ch == stop[i]);
				// stop[0..i-1] found; continue with last read character
				if (generateOutput) {
					std::wstring subStop = CocoUtil::coco_string_create(stop, 0, i);
					fwprintf(gen, L"%ls", subStop.c_str());
					CocoUtil::coco_string_delete(subStop);
				}
			} else {
				if (generateOutput) { fwprintf(gen, L"%lc", ch); }
				fwscanf(fram, L"%lc", &ch);
			}
		}
		if (stop.length() != 0) {

			std::wstringstream wss;
			wss << L" -- Incomplete or corrupt frame file: " << frameFile << " at " << stop << "\n";

			errors->Exception(wss.str().c_str());
		}
	}

}