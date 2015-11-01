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


#include <memory.h>
#include <string.h>
#include "Scanner.h"

namespace Coco {



// string handling, wide character


char* coco_string_create(const char* value) {
	return coco_string_create(value, 0);
}

char* coco_string_create(const char *value, int startIndex) {
	int valueLen = 0;
	int len = 0;

	if (value) {
		valueLen = strlen(value);
		len = valueLen - startIndex;
	}

	return coco_string_create(value, startIndex, len);
}

char* coco_string_create(const char *value, int startIndex, int length) {
	int len = 0;
	char* data;

	if (value) { len = length; }
	data = new char[len + 1];
	strncpy(data, &(value[startIndex]), len);
	data[len] = 0;

	return data;
}

char* coco_string_create_upper(const char* data) {
	if (!data) { return NULL; }

	int dataLen = 0;
	if (data) { dataLen = strlen(data); }

	char *newData = new char[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		if (('a' <= data[i]) && (data[i] <= 'z')) {
			newData[i] = data[i] + ('A' - 'a');
		}
		else { newData[i] = data[i]; }
	}

	newData[dataLen] = '\0';
	return newData;
}

char* coco_string_create_lower(const char* data) {
	if (!data) { return NULL; }
	int dataLen = strlen(data);
	return coco_string_create_lower(data, 0, dataLen);
}

char* coco_string_create_lower(const char* data, int startIndex, int dataLen) {
	if (!data) { return NULL; }

	char* newData = new char[dataLen + 1];

	for (int i = 0; i <= dataLen; i++) {
		char ch = data[startIndex + i];
		if (('A' <= ch) && (ch <= 'Z')) {
			newData[i] = ch - ('A' - 'a');
		}
		else { newData[i] = ch; }
	}
	newData[dataLen] = '\0';
	return newData;
}

char* coco_string_create_append(const char* data1, const char* data2) {
	char* data;
	int data1Len = 0;
	int data2Len = 0;

	if (data1) { data1Len = strlen(data1); }
	if (data2) {data2Len = strlen(data2); }

	data = new char[data1Len + data2Len + 1];

	if (data1) { strcpy(data, data1); }
	if (data2) { strcpy(data + data1Len, data2); }

	data[data1Len + data2Len] = 0;

	return data;
}

char* coco_string_create_append(const char *target, const char appendix) {
	int targetLen = coco_string_length(target);
	char* data = new char[targetLen + 2];
	strncpy(data, target, targetLen);
	data[targetLen] = appendix;
	data[targetLen + 1] = 0;
	return data;
}

void coco_string_delete(char* &data) {
	delete [] data;
	data = NULL;
}

int coco_string_length(const char* data) {
	if (data) { return strlen(data); }
	return 0;
}

bool coco_string_endswith(const char* data, const char *end) {
	int dataLen = strlen(data);
	int endLen = strlen(end);
	return (endLen <= dataLen) && (strcmp(data + dataLen - endLen, end) == 0);
}

int coco_string_indexof(const char* data, const char value) {
	const char* chr = strchr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

int coco_string_lastindexof(const char* data, const char value) {
	const char* chr = strrchr(data, value);

	if (chr) { return (chr-data); }
	return -1;
}

void coco_string_merge(char* &target, const char* appendix) {
	if (!appendix) { return; }
	char* data = coco_string_create_append(target, appendix);
	delete [] target;
	target = data;
}

bool coco_string_equal(const char* data1, const char* data2) {
	return strcmp( data1, data2 ) == 0;
}

int coco_string_compareto(const char* data1, const char* data2) {
	return strcmp(data1, data2);
}

int coco_string_hash(const char *data) {
	int h = 0;
	if (!data) { return 0; }
	while (*data != 0) {
		h = (h * 7) ^ *data;
		++data;
	}
	if (h < 0) { h = -h; }
	return h;
}

Token::Token() {
	kind = 0;
	pos  = 0;
	col  = 0;
	line = 0;
	val  = NULL;
	next = NULL;
}

Token::~Token() {
	coco_string_delete(val);
}

Buffer::Buffer(FILE* s, bool isUserStream) {
// ensure binary read on windows
#if _MSC_VER >= 1300
	_setmode(_fileno(s), _O_BINARY);
#endif
	stream = s; this->isUserStream = isUserStream;
	if (CanSeek()) {
		fseek(s, 0, SEEK_END);
		fileLen = ftell(s);
		fseek(s, 0, SEEK_SET);
		bufLen = (fileLen < COCO_MAX_BUFFER_LENGTH) ? fileLen : COCO_MAX_BUFFER_LENGTH;
		bufStart = INT_MAX; // nothing in the buffer so far
	} else {
		fileLen = bufLen = bufStart = 0;
	}
	bufCapacity = (bufLen>0) ? bufLen : COCO_MIN_BUFFER_LENGTH;
	buf = new unsigned char[bufCapacity];	
	if (fileLen > 0) SetPos(0);          // setup  buffer to position 0 (start)
	else bufPos = 0; // index 0 is already after the file, thus Pos = 0 is invalid
	if (bufLen == fileLen && CanSeek()) Close();
}

Buffer::Buffer(Buffer *b) {
	buf = b->buf;
	bufCapacity = b->bufCapacity;
	b->buf = NULL;
	bufStart = b->bufStart;
	bufLen = b->bufLen;
	fileLen = b->fileLen;
	bufPos = b->bufPos;
	stream = b->stream;
	b->stream = NULL;
	isUserStream = b->isUserStream;
}

Buffer::Buffer(const char* buf, int len) {
	this->buf = new unsigned char[len];
	memcpy(this->buf, buf, len*sizeof(char));
	bufStart = 0;
	bufCapacity = bufLen = len;
	fileLen = len;
	bufPos = 0;
	stream = NULL;
}

Buffer::~Buffer() {
	Close(); 
	if (buf != NULL) {
		delete [] buf;
		buf = NULL;
	}
}

void Buffer::Close() {
	if (!isUserStream && stream != NULL) {
		fclose(stream);
		stream = NULL;
	}
}

int Buffer::Read() {
	if (bufPos < bufLen) {
		return buf[bufPos++];
	} else if (GetPos() < fileLen) {
		SetPos(GetPos()); // shift buffer start to Pos
		return buf[bufPos++];
	} else if ((stream != NULL) && !CanSeek() && (ReadNextStreamChunk() > 0)) {
		return buf[bufPos++];
	} else {
		return EoF;
	}
}

int Buffer::Peek() {
	int curPos = GetPos();
	int ch = Read();
	SetPos(curPos);
	return ch;
}

// beg .. begin, zero-based, inclusive, in byte
// end .. end, zero-based, exclusive, in byte
char* Buffer::GetString(int beg, int end) {
	int len = 0;
	char *buf = new char[end - beg];
	int oldPos = GetPos();
	SetPos(beg);
	while (GetPos() < end) buf[len++] = (char) Read();
	SetPos(oldPos);
	char *res = coco_string_create(buf, 0, len);
	coco_string_delete(buf);
	return res;
}

int Buffer::GetPos() {
	return bufPos + bufStart;
}

void Buffer::SetPos(int value) {
	if ((value >= fileLen) && (stream != NULL) && !CanSeek()) {
		// Wanted position is after buffer and the stream
		// is not seek-able e.g. network or console,
		// thus we have to read the stream manually till
		// the wanted position is in sight.
		while ((value >= fileLen) && (ReadNextStreamChunk() > 0));
	}

	if ((value < 0) || (value > fileLen)) {
		fprintf(stderr, "--- buffer out of bounds access, position: %d\n", value);
		exit(1);
	}

	if ((value >= bufStart) && (value < (bufStart + bufLen))) { // already in buffer
		bufPos = value - bufStart;
	} else if (stream != NULL) { // must be swapped in
		fseek(stream, value, SEEK_SET);
		bufLen = fread(buf, sizeof(char), bufCapacity, stream);
		bufStart = value; bufPos = 0;
	} else {
		bufPos = fileLen - bufStart; // make Pos return fileLen
	}
}

// Read the next chunk of bytes from the stream, increases the buffer
// if needed and updates the fields fileLen and bufLen.
// Returns the number of bytes read.
int Buffer::ReadNextStreamChunk() {
	int free = bufCapacity - bufLen;
	if (free == 0) {
		// in the case of a growing input stream
		// we can neither seek in the stream, nor can we
		// foresee the maximum length, thus we must adapt
		// the buffer size on demand.
		bufCapacity = bufLen * 2;
		unsigned char *newBuf = new unsigned char[bufCapacity];
		memcpy(newBuf, buf, bufLen*sizeof(char));
		delete [] buf;
		buf = newBuf;
		free = bufLen;
	}
	int read = fread(buf + bufLen, sizeof(char), free, stream);
	if (read > 0) {
		fileLen = bufLen = (bufLen + read);
		return read;
	}
	// end of stream reached
	return 0;
}

bool Buffer::CanSeek() {
	return (stream != NULL) && (ftell(stream) != -1);
}

Scanner::Scanner(const char* buf, int len) {
	buffer = new Buffer(buf, len);
	Init();
}

Scanner::Scanner(const char* fileName) {
	FILE* stream;
	char *chFileName = coco_string_create(fileName);
	if ((stream = fopen(chFileName, "rb")) == NULL) {
		fprintf(stderr, "--- Cannot open file %s\n", fileName);
		exit(1);
	}
	coco_string_delete(chFileName);
	buffer = new Buffer(stream, false);
	Init();
}

Scanner::Scanner(FILE* s) {
	buffer = new Buffer(s, true);
	Init();
}

Scanner::~Scanner() {
	char* cur = (char*) firstHeap;

	while(cur != NULL) {
		cur = *(char**) (cur + COCO_HEAP_BLOCK_SIZE);
		free(firstHeap);
		firstHeap = cur;
	}
	delete [] tval;
	delete buffer;
}

void Scanner::Init() {
	EOL    = '\n';
	eofSym = 0;
	maxT = 41;
	noSym = 41;
	int i;
	for (i = 65; i <= 90; ++i) start.set(i, 1);
	for (i = 95; i <= 95; ++i) start.set(i, 1);
	for (i = 97; i <= 122; ++i) start.set(i, 1);
	for (i = 48; i <= 57; ++i) start.set(i, 2);
	start.set(34, 12);
	start.set(39, 5);
	start.set(36, 13);
	start.set(61, 16);
	start.set(46, 31);
	start.set(43, 17);
	start.set(45, 18);
	start.set(60, 32);
	start.set(62, 20);
	start.set(124, 23);
	start.set(40, 33);
	start.set(41, 24);
	start.set(91, 25);
	start.set(93, 26);
	start.set(123, 27);
	start.set(125, 28);
		start.set(Buffer::EoF, -1);
	keywords.set("COMPILER", 6);
	keywords.set("IGNORECASE", 7);
	keywords.set("CHARACTERS", 8);
	keywords.set("TOKENS", 9);
	keywords.set("PRAGMAS", 10);
	keywords.set("COMMENTS", 11);
	keywords.set("FROM", 12);
	keywords.set("TO", 13);
	keywords.set("NESTED", 14);
	keywords.set("IGNORE", 15);
	keywords.set("PRODUCTIONS", 16);
	keywords.set("END", 19);
	keywords.set("ANY", 23);
	keywords.set("WEAK", 29);
	keywords.set("SYNC", 36);
	keywords.set("IF", 37);
	keywords.set("CONTEXT", 38);


	tvalLength = 128;
	tval = new char[tvalLength]; // text of current token

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	heap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	firstHeap = heap;
	heapEnd = (void**) (((char*) heap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heapTop = heap;
	assert(sizeof(Token) < COCO_HEAP_BLOCK_SIZE);

	pos = -1; line = 1; col = 0; charPos = -1;
	oldEols = 0;
	NextCh();


	pt = tokens = CreateToken(); // first token is a dummy
}

void Scanner::NextCh() {
	if (oldEols > 0) { ch = EOL; oldEols--; }
	else {
		pos = buffer->GetPos();
		ch = buffer->Read(); col++; charPos++;
		// replace isolated '\r' by '\n' in order to make
		// eol handling uniform across Windows, Unix and Mac
		if (ch == '\r' && buffer->Peek() != '\n') ch = EOL;
		if (ch == EOL) { line++; col = 0; }
	}

}

void Scanner::AddCh() {
	if (tlen >= tvalLength) {
		tvalLength *= 2;
		char *newBuf = new char[tvalLength];
		memcpy(newBuf, tval, tlen*sizeof(char));
		delete [] tval;
		tval = newBuf;
	}
	if (ch != Buffer::EoF) {
		tval[tlen++] = (char)ch;
		NextCh();
	}
}


bool Scanner::Comment0() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == '/') {
		NextCh();
		for(;;) {
			if (ch == 10) {
				level--;
				if (level == 0) { oldEols = line - line0; NextCh(); return true; }
				NextCh();
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}

bool Scanner::Comment1() {
	int level = 1, pos0 = pos, line0 = line, col0 = col, charPos0 = charPos;
	NextCh();
	if (ch == '*') {
		NextCh();
		for(;;) {
			if (ch == '*') {
				NextCh();
				if (ch == '/') {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == '/') {
				NextCh();
				if (ch == '*') {
					level++; NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0; charPos = charPos0;
	}
	return false;
}


void Scanner::CreateHeapBlock() {
	void* newHeap;
	char* cur = (char*) firstHeap;

	while(((char*) tokens < cur) || ((char*) tokens > (cur + COCO_HEAP_BLOCK_SIZE))) {
		cur = *((char**) (cur + COCO_HEAP_BLOCK_SIZE));
		free(firstHeap);
		firstHeap = cur;
	}

	// COCO_HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	newHeap = malloc(COCO_HEAP_BLOCK_SIZE + sizeof(void*));
	*heapEnd = newHeap;
	heapEnd = (void**) (((char*) newHeap) + COCO_HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heap = newHeap;
	heapTop = heap;
}

Token* Scanner::CreateToken() {
	Token *t;
	if (((char*) heapTop + (int) sizeof(Token)) >= (char*) heapEnd) {
		CreateHeapBlock();
	}
	t = (Token*) heapTop;
	heapTop = (void*) ((char*) heapTop + sizeof(Token));
	t->val = NULL;
	t->next = NULL;
	return t;
}

void Scanner::AppendVal(Token *t) {
	int reqMem = (tlen + 1) * sizeof(char);
	if (((char*) heapTop + reqMem) >= (char*) heapEnd) {
		assert(reqMem < COCO_HEAP_BLOCK_SIZE);
		CreateHeapBlock();
	}
	t->val = (char*) heapTop;
	heapTop = (void*) ((char*) heapTop + reqMem);

	strncpy(t->val, tval, tlen);
	t->val[tlen] = '\0';
}

Token* Scanner::NextToken() {
	while (ch == ' ' ||
			(ch >= 9 && ch <= 10) || ch == 13
	) NextCh();
	if ((ch == '/' && Comment0()) || (ch == '/' && Comment1())) return NextToken();
	int recKind = noSym;
	int recEnd = pos;
	t = CreateToken();
	t->pos = pos; t->col = col; t->line = line; t->charPos = charPos;
	int state = start.state(ch);
	tlen = 0; AddCh();

	switch (state) {
		case -1: { t->kind = eofSym; break; } // NextCh already done
		case 0: {
			case_0:
			if (recKind != noSym) {
				tlen = recEnd - t->pos;
				SetScannerBehindT();
			}
			t->kind = recKind; break;
		} // NextCh already done
		case 1:
			case_1:
			recEnd = pos; recKind = 1;
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_1;}
			else {t->kind = 1; char *literal = coco_string_create(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 2:
			case_2:
			recEnd = pos; recKind = 2;
			if ((ch >= '0' && ch <= '9')) {AddCh(); goto case_2;}
			else {t->kind = 2; break;}
		case 3:
			case_3:
			{t->kind = 3; break;}
		case 4:
			case_4:
			{t->kind = 4; break;}
		case 5:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= '&') || (ch >= '(' && ch <= '[') || (ch >= ']' && ch <= 255)) {AddCh(); goto case_6;}
			else if (ch == 92) {AddCh(); goto case_7;}
			else {goto case_0;}
		case 6:
			case_6:
			if (ch == 39) {AddCh(); goto case_9;}
			else {goto case_0;}
		case 7:
			case_7:
			if ((ch >= ' ' && ch <= '~')) {AddCh(); goto case_8;}
			else {goto case_0;}
		case 8:
			case_8:
			if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')) {AddCh(); goto case_8;}
			else if (ch == 39) {AddCh(); goto case_9;}
			else {goto case_0;}
		case 9:
			case_9:
			{t->kind = 5; break;}
		case 10:
			case_10:
			recEnd = pos; recKind = 42;
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_10;}
			else {t->kind = 42; break;}
		case 11:
			case_11:
			recEnd = pos; recKind = 43;
			if ((ch >= '-' && ch <= '.') || (ch >= '0' && ch <= ':') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_11;}
			else {t->kind = 43; break;}
		case 12:
			case_12:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= '!') || (ch >= '#' && ch <= '[') || (ch >= ']' && ch <= 255)) {AddCh(); goto case_12;}
			else if (ch == 10 || ch == 13) {AddCh(); goto case_4;}
			else if (ch == '"') {AddCh(); goto case_3;}
			else if (ch == 92) {AddCh(); goto case_14;}
			else {goto case_0;}
		case 13:
			recEnd = pos; recKind = 42;
			if ((ch >= '0' && ch <= '9')) {AddCh(); goto case_10;}
			else if ((ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_15;}
			else {t->kind = 42; break;}
		case 14:
			case_14:
			if ((ch >= ' ' && ch <= '~')) {AddCh(); goto case_12;}
			else {goto case_0;}
		case 15:
			case_15:
			recEnd = pos; recKind = 42;
			if ((ch >= '0' && ch <= '9')) {AddCh(); goto case_10;}
			else if ((ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_15;}
			else if (ch == '=') {AddCh(); goto case_11;}
			else {t->kind = 42; break;}
		case 16:
			{t->kind = 17; break;}
		case 17:
			{t->kind = 20; break;}
		case 18:
			{t->kind = 21; break;}
		case 19:
			case_19:
			{t->kind = 22; break;}
		case 20:
			{t->kind = 25; break;}
		case 21:
			case_21:
			{t->kind = 26; break;}
		case 22:
			case_22:
			{t->kind = 27; break;}
		case 23:
			{t->kind = 28; break;}
		case 24:
			{t->kind = 31; break;}
		case 25:
			{t->kind = 32; break;}
		case 26:
			{t->kind = 33; break;}
		case 27:
			{t->kind = 34; break;}
		case 28:
			{t->kind = 35; break;}
		case 29:
			case_29:
			{t->kind = 39; break;}
		case 30:
			case_30:
			{t->kind = 40; break;}
		case 31:
			recEnd = pos; recKind = 18;
			if (ch == '.') {AddCh(); goto case_19;}
			else if (ch == '>') {AddCh(); goto case_22;}
			else if (ch == ')') {AddCh(); goto case_30;}
			else {t->kind = 18; break;}
		case 32:
			recEnd = pos; recKind = 24;
			if (ch == '.') {AddCh(); goto case_21;}
			else {t->kind = 24; break;}
		case 33:
			recEnd = pos; recKind = 30;
			if (ch == '.') {AddCh(); goto case_29;}
			else {t->kind = 30; break;}

	}
	AppendVal(t);
	return t;
}

void Scanner::SetScannerBehindT() {
	buffer->SetPos(t->pos);
	NextCh();
	line = t->line; col = t->col; charPos = t->charPos;
	for (int i = 0; i < tlen; i++) NextCh();
}

// get the next token (possibly a token already seen during peeking)
Token* Scanner::Scan() {
	if (tokens->next == NULL) {
		return pt = tokens = NextToken();
	} else {
		pt = tokens = tokens->next;
		return tokens;
	}
}

// peek for the next token, ignore pragmas
Token* Scanner::Peek() {
	do {
		if (pt->next == NULL) {
			pt->next = NextToken();
		}
		pt = pt->next;
	} while (pt->kind > maxT); // skip pragmas

	return pt;
}

// make sure that peeking starts at the current scan position
void Scanner::ResetPeek() {
	pt = tokens;
}

} // namespace

