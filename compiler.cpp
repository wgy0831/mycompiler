#include "compiler.h"
bool operator==(Quanode &a, Quanode &b) {
	if (a.qt != b.qt) return false;
	if (a.qt == IMM || a.qt == REG) return a.num == b.num;
	if (a.qt == CHA) return a.sl == b.sl;
	if (a.qt == b.qt) return true;
	return false;
}
void error(int errcode) {
	errored = true;
	fprintf(stderr,"Line: %d\tcolumn:%d\t", get.row, get.column);
	switch (get.nw) {
		case ADD:
			fprintf(stderr,"ADD +\t");
			break;
		case MINUS:
			fprintf(stderr,"MINUS -\t");
			break;
		case MULTI:
			fprintf(stderr,"MULTI *\t");
			break;
		case DIV:
			fprintf(stderr,"DIV /\t");
			break;
		case SEMIC:
			fprintf(stderr,"SEMIC ;\t");
			break;
		case COMMA:
			fprintf(stderr,"COMMA ,\t");
			break;
		case LLINE:
			fprintf(stderr,"LEFTLINE {\t");
			break;
		case RLINE:
			fprintf(stderr,"RIGHTLINE }\t");
			break;
		case LARR:
			fprintf(stderr,"LARR [\t");
			break;
		case RARR:
			fprintf(stderr,"RARR ]\t");
			break;
		case COLON:
			fprintf(stderr,"COLON :\t");
			break;
		case LBRA:
			fprintf(stderr,"LBRA (\t");
			break;
		case RBRA:
			fprintf(stderr,"RBRA )\t");
			break;
		case LESSE:
			fprintf(stderr,"LESS OR EQUAL <=\t");
			break;
		case LESS:
			fprintf(stderr,"LESS <\t");
			break;
		case MOREE:
			fprintf(stderr,"MORE OR EQUAL >=\t");
			break;
		case MORE:
			fprintf(stderr,"MORE >\t");
			break;
		case NOTE:
			fprintf(stderr,"NOT EQUAL !=\t");
			break;
		case EQUAL:
			fprintf(stderr,"EQUAL ==\t");
			break;
		case ASSIG:
			fprintf(stderr,"ASSIGN =\t");
			break;
		case ALPHA:
			fprintf(stderr,"ALPHA %c\t", get.ch);
			break;
		case STRING:
			fprintf(stderr,"STRING %s\t", strtab.back().c_str());
			break;
		case NUMBER:
			fprintf(stderr,"NUMBER %d\t", get.number);
			break;
		case CONST:
			fprintf(stderr,"CONST const\t");
			break;
		case TYPEI:
			fprintf(stderr,"TYPEI int\t");
			break;
		case TYPEC:
			fprintf(stderr,"TYPEC char\t");
			break;
		case TYPEV:
			fprintf(stderr,"TYPEV void\t");
			break;
		case IF:
			fprintf(stderr,"IF if\t");
			break;
		case DO:
			fprintf(stderr,"DO do\t");
			break;
		case WHILE:
			fprintf(stderr,"WHILE while\t");
			break;
		case CASE:
			fprintf(stderr,"CASE case\t");
			break;
		case SWITCH:
			fprintf(stderr,"SWITCH switch\t");
			break;
		case RETURN:
			fprintf(stderr,"RETURN return\t");
			break;
		case PRINTF:
			fprintf(stderr,"PRINTF printf\t");
			break;
		case SCANF:
			fprintf(stderr,"SCANF scanf\t");
			break;
		case MAIN:
			fprintf(stderr,"MAIN main\t");
			break;
		case IDENT:
			fprintf(stderr,"IDENT %s\t", get.identifier.c_str());
			break;
		case ERROR:
			fprintf(stderr,"ERROR\t");
			break;
		case WORDNU:
			fprintf(stderr,"WORDNU unrecogize word\t");
	}
	fprintf(stderr,"%s\n", errormsg[errcode]);
}
Word insymbol() {
	char c, nc, data;
	bool illegal;
	int lastp;
	std::string str;
	while(isspace(c = getc(input))) {
		if (c != '\n')
			++get.column;
		else {
			get.column = 1;
			++get.row;
		}
	}
	++get.column;
	switch (c) {
		case  -1: return WORDNU;
		case '+': return ADD;
		case '-': return MINUS;
		case '*': return MULTI;
		case '/': return DIV;
		case ';': return SEMIC;
		case ',': return COMMA;
		case '{': return LLINE;
		case '}': return RLINE;
		case '[': return LARR;
		case ']': return RARR;
		case ':': return COLON;
		case '(': return LBRA;
		case ')': return RBRA;
		case '<':
			nc = getc(input);
			if (nc == '=') {
				++get.column;
				return LESSE;
			}
			else {
				ungetc(nc, input);
				return LESS;
			}
		case '>':
			nc = getc(input);
			if (nc == '=') {
				++get.column;
				return MOREE;
			}
			else {
				ungetc(nc, input);
				return MORE;
			}
		case '!':
			nc = getc(input);
			if (nc == '=') {
				++get.column;
				return NOTE;
			}
			else {
				ungetc(nc, input);
				return ERROR;
			}
		case '=':
			nc = getc(input);
			if (nc == '=') {
				++get.column;
				return EQUAL;
			}
			else {
				ungetc(nc, input);
				return ASSIG;
			}
		case '\'':
			data = getc(input);
			++get.column;
			illegal = true;
			if (isalnum(data) || data == '_' || data == '+' || data == '-' || data == '*' || data == '/')
				illegal = false;
			get.ch = data;
			if (data != '\'') {
				data = getc(input);
				++get.column;
				if (data != '\'') {
					ungetc(data, input);
					illegal = true;
					--get.column;
				}
			}
			if (illegal) return ERROR;
			else return ALPHA;
		case '"':
			str = "";
			illegal = false;
			while((c = getc(input)) != '"' && c != '\n') {
				++get.column;
				if (c < 32 || c > 126 || c == 34) illegal = true;
				str += c;
				//if (c == '\\') str += '\\';
			}
			if (c == '\n'){
				++get.row;
				get.column = 0;
				return ERROR;
			}
			else if (c == '"') {
				++get.column;
				if (!illegal) {
					strtab.push_back(str);
					return STRING;
				}
				else return ERROR;
			}
		default:
			if (isdigit(c)) {
				get.number = c - '0';
				lastp = get.column;
				nc = c;
				while (isdigit(c = getc(input))) {
					++get.column;
					get.number = get.number*10 + c - '0';
				}
				ungetc(c, input);
				return (nc == '0' && lastp != get.column)?ERROR: NUMBER;
			}
			else if (isalpha(c) || c == '_') {
				get.identifier = tolower(c);
				while(isalnum(c = getc(input)) || c == '_') {
					++get.column;
					get.identifier += tolower(c);
				}
				ungetc(c, input);
				if (get.identifier == "const")
					return CONST;
				else if (get.identifier == "int")
					return TYPEI;
				else if (get.identifier == "char")
					return TYPEC;
				else if (get.identifier == "void")
					return TYPEV;
				else if (get.identifier == "if")
					return IF;
				else if (get.identifier == "do")
					return DO;
				else if (get.identifier == "while")
					return WHILE;
				else if (get.identifier == "case")
					return CASE;
				else if (get.identifier == "switch")
					return SWITCH;
				else if (get.identifier == "return")
					return RETURN;
				else if (get.identifier == "printf")
					return PRINTF;
				else if (get.identifier == "scanf")
					return SCANF;
				else if (get.identifier == "main")
					return MAIN;
				else return IDENT;
			}
	}
	return ERROR;
}
void addqua(Quaop qop,const Quanode &re,const Quanode &nu1,const Quanode &nu2) {
	if (errored) return;
	Qua q;
	q.op = qop;
	q.re = re;
	q.nu1 = nu1;
	q.nu2 = nu2;
	q.blockstart = 0;
	midcodes.push_back(q);
}
inline void skip() {
	while(get.nw != SEMIC) get.nw = insymbol();
	while(!wordskip[get.nw])
		get.nw = insymbol();
}
const Quanode reg_top() {
	static int count = 0;
	Quanode q;
	q.qt = REG;
	q.num = ++count;
	return q;
}
void addsys(unsigned int index, std::string name, Symbolline &data){
	data.index = index;
	data.addr = 0xffffffff;
	data.count = 0;
	//data.count = 0;
	if (systab[index].find(name) != systab[index].end()) {
		error(20);
		return;
	}
	Quanode cha, tmp;
	cha.qt = CHA;
	cha.sl = systab[index].insert(std::make_pair(name, data)).first;
	switch (data.ki) {
		case CON:
			addqua(constop, null, cha, null);
			break;
		case VAR:
			addqua(varop, null, cha, null);
			break;
		case ARRAY:
			tmp.qt = IMM;
			tmp.num = data.exl.arrlimit;
			addqua(arrdefop, null, cha, tmp);
			break;
		case FUNCHEAD:
			addqua(funcdecop, null, cha, null);
			index2sys.push_back(cha.sl);
			break;
		default:
			break;
	}
}
inline bool search(unsigned int index, std::string name, Hashmap::iterator &sl) {
	Symbolline zerosy = {NOKIND, NOTYPE, 0}; 
	sl = systab[index].find(name);
	if (sl == systab[index].end()) {
		if (index == 0) {
			addsys(index, name, zerosy);
			error(11);
			return false;   //need addsys(nokind, notype)?
		}
		else {
			sl = systab[0].find(name);
			if (sl == systab[0].end()){
				addsys(index, name, zerosy);
				error(11);
				return false;  //the same
			}
		}
	}
	return true;
}
void conststatment(unsigned int index) {
	memset(wordskip, 0, sizeof(wordskip));
	wordskip[TYPEI] = true;
	wordskip[TYPEC] = true;
	wordskip[CONST] = true;
	if (index == 0) {   //global skip
		wordskip[TYPEV] = true;
	}
	else {        //func skip
		wordskip[IF] = true;
		wordskip[DO] = true;
		wordskip[SWITCH] = true;
		wordskip[SCANF] = true;
		wordskip[PRINTF] = true;
		wordskip[RETURN] = true;
	}
	while (get.nw == CONST) {
		Type ty;
		get.nw = insymbol();
		if (get.nw == TYPEC) {
			ty = CHAR;
			get.nw = insymbol();
		}
		else if(get.nw == TYPEI) {
			ty = INT;
			get.nw = insymbol();
		}
		else {
			error(2);
			wordskip[SEMIC] = false;
			skip();
			continue;
		}
		wordskip[SEMIC] = true;
		while (true){
			if (get.nw != IDENT) {
				error(16);
				skip();
				break;
			}
			std::string name = get.identifier;
			if((get.nw = insymbol()) != ASSIG) {
				error(4);
				skip();
				break;
			}
			else {
				get.nw = insymbol();
				int sinum;
				if (ty == INT) {
					int sign = get.nw == MINUS ? -1 :1;
					if (get.nw == MINUS || get.nw == ADD)
						get.nw = insymbol();
					if (get.nw != NUMBER) {
						error(5);
						skip();
						break;
					}
					sinum = sign*get.number;
				}
				else {
					if (get.nw != ALPHA) {
						error(3);
						skip();
						break;
					}
				}
				Symbolline sy = {CON, ty};
				sy.exl.cn = ty == CHAR?get.ch:sinum;
				addsys(index, name, sy);
				//addsys(index, name, CON, ty,(void *)(ty==CHAR?get.ch:get.number));
				get.nw = insymbol();
			}
			if (get.nw == SEMIC)
				break;
			if (get.nw != COMMA) {
				error(6);
				skip();
				break;
			}
			else get.nw = insymbol();
		}
		if (get.nw != SEMIC)
			error(7);
		else get.nw = insymbol();
	//	printf("this is const statment!\n");
	}
}
void varstatment(unsigned int index) {
	memset(wordskip, 0, sizeof(wordskip));
	wordskip[TYPEI] = true;
	wordskip[TYPEC] = true;
	wordskip[SEMIC] = true;
	if (index == 0) {   //global skip
		wordskip[TYPEV] = true;
	}
	else {        //func skip
		wordskip[LLINE] = true;
		wordskip[IF] = true;
		wordskip[DO] = true;
		wordskip[SWITCH] = true;
		wordskip[SCANF] = true;
		wordskip[PRINTF] = true;
		wordskip[RETURN] = true;
	}
	while(get.nw == TYPEI || get.nw == TYPEC) {
		Type ty = get.nw == TYPEI? INT:CHAR;
		get.nw = insymbol();
		bool first = true;
		while (true) {
			if (get.nw != IDENT) {
				error(16);
				skip();
				break;
			}
			std::string name = get.identifier;
			if ((get.nw = insymbol()) == LARR) {
				if ((get.nw = insymbol()) != NUMBER) {
					error(5);
					skip();
					break;
				}
				get.nw = insymbol();
				if (get.nw != RARR) {
					error(21);
					skip();
					break;
				}
				else get.nw = insymbol();
				Symbolline sy = {ARRAY, ty};
				sy.exl.arrlimit = get.number;
				addsys(index, name, sy);
				//addsys(index, name, ARRAY, ty,(void *)get.number);
			}
			else if (index == 0 && first && get.nw == LBRA) {
				get.nw = insymbol();
				funchead(ty, name);
				return;
			}
			else if (get.nw == COMMA || get.nw == SEMIC) {
				Symbolline sy = {VAR, ty};
				addsys(index, name, sy);
				//addsys(index, name, VAR, ty, 0);
				if (get.nw == SEMIC)
					break;
			}
			else {
				error(6);
				skip();
				break;
			}
			first = false;
			if (get.nw == SEMIC)
				break;
			if (get.nw != COMMA) {
				error(6);
				skip();
				break;
			}
			else get.nw = insymbol();
		}
		if (get.nw != SEMIC)
			error(7);
		else get.nw = insymbol();
	//	printf("this is var statment!\n");
	}
}
void funcdec() {
	bool finished = false;
	memset(wordskip, 0, sizeof(wordskip));
	while(get.nw == TYPEV || get.nw == TYPEI || get.nw == TYPEC) {
		wordskip[LLINE] = true;
		Type ty = NOTYPE;
		switch(get.nw) {
			case TYPEV:
				ty = VOID;
				break;
			case TYPEI:
				ty = INT;
				break;
			case TYPEC:
				ty = CHAR;
				break;
			default:
				ty = NOTYPE;
		}
		get.nw = insymbol();
		std::string name = get.identifier;
		if (get.nw == MAIN && ty == VOID) {
			name = "main";
			finished = true;
			get.nw = insymbol();
			if (get.nw == LBRA) 
				get.nw = insymbol();
			else {
				error(9);
				skip();
			}
			funchead(VOID, name);
			break;
		}
		if (get.nw != IDENT) {
			error(16);
			name = "";
		}
		else get.nw = insymbol();
		if (get.nw == LBRA)  {
			get.nw = insymbol();
		}
		else {
			error(9);
			skip();
		}
		funchead(ty, name);
	}
	if (!finished)
		error(25);
}
void funchead(Type ty, std::string name) {
	std::vector<Type> tyarr;
	Symbolline sy;
	//sy.name = name;
	sy.ki = FUNCHEAD;
	sy.ty = ty;
	Functab *ftab = new Functab;
	Hashmap em;
	systab.push_back(em);
	int pointer = ftab->pointer = systab.size()-1;
	if (name != ""){
		Symbolline sy = {FUNCHEAD, ty, ftab};
		addsys(0, name, sy);
		//addsys(0, name, FUNCHEAD, ty, (void *)ftab);
	}
	wordskip[TYPEI] = true;
	wordskip[TYPEC] = true;
	wordskip[RBRA] = true;
	if (name != "main")
	while(get.nw == TYPEI || get.nw == TYPEC) {
		Type ty = get.nw == TYPEI? INT:CHAR;
		tyarr.push_back(ty);
		get.nw = insymbol();
		if (get.nw != IDENT) {
			error(16);
			skip();
			continue;
		}
		std::string name = get.identifier;
		Symbolline sy = {VAR, ty};
		addqua(paradec, null, null, null);
		addsys(pointer, name, sy);
		//addsys(pointer, name, VAR, ty, 0);
		get.nw = insymbol();
		if (get.nw == RBRA)
			break;
		if (get.nw != COMMA)
			error(6);
		else get.nw = insymbol();
	}
	int size = tyarr.size();
	ftab->para = new Type[size];
	ftab->size = size;
	for(int i = 0; i < size; ++i)
		ftab->para[i] = tyarr[i];
	if (get.nw == RBRA) 
		get.nw = insymbol();
	else error(18);
	if (get.nw == LLINE)
		get.nw = insymbol();
	else error(10);
	conststatment(pointer);
	varstatment(pointer);
	returned = false;
	statments(pointer);
	if (get.nw == RLINE)
		get.nw = insymbol();
	else  {
		memset(wordskip, 0, sizeof(wordskip));
		wordskip[RLINE] = true;
		error(19);
	}
	if (sy.ty != VOID) {
		if (!returned) error(22);
	}
	addqua(nop, null, null, null);
	//else if (!returned) addqua(retop, null, null, null);
//	printf("this is function statment!\n");
}
bool statments(unsigned int index) {
	memset(wordskip, 0, sizeof(wordskip));
	wordskip[LLINE] = true;
	wordskip[IF] = true;
	wordskip[DO] = true;
	wordskip[SWITCH] = true;
	wordskip[SCANF] = true;
	wordskip[PRINTF] = true;
	wordskip[RETURN] = true;
	wordskip[IDENT] = true;
	Word lawd;
	bool reted = false;
	while((lawd = statment(index)) != WORDNU)
		if (lawd == RETURN) reted = true;
//	printf("this is statments!\n");
	return reted;
}
Word statment(unsigned int index) {
	Word lastwd = get.nw;
	switch(get.nw) {
		case IF:
			get.nw = insymbol();
			ifstatment(index);
			break;
		case DO:
			get.nw = insymbol();
			dostatment(index);
			break;
		case SWITCH:
			get.nw = insymbol();
			switchstatment(index);
			break;
		case LLINE:
			get.nw = insymbol();
			statments(index);
			if (get.nw != RLINE) {
				error(19);
				skip();
			}
			else get.nw = insymbol();
			break;
		case IDENT:
			castatment(index);
			break;
		case SCANF:
			get.nw = insymbol();
			scanfstatment(index);
			break;
		case PRINTF:
			get.nw = insymbol();
			printfstatment(index);
			break;
		case SEMIC:
			get.nw = insymbol();
			break;
		case RETURN:
			get.nw = insymbol();
			returnstatment(index);
			break;
		default:
			return WORDNU;
	}
	return lastwd;
}
//mess starts
void condition(unsigned int index) {
	Quanode first, second;
	expression(index, first);
	Word tmp = get.nw;
	if (tmp == LESS || tmp == LESSE || tmp == MORE ||
		tmp == MOREE || tmp == NOTE || tmp == EQUAL) {
		get.nw = insymbol();
		expression(index, second);
		Quaop qop = equalop;
		bool bimm;
		//if (!errored)
		switch(tmp) {
			case LESS:
				qop = lessop;
				bimm = first.num < second.num;
				break;
			case LESSE:
				qop = lesseop;
				bimm = first.num <= second.num;
				break;
			case MORE:
				qop = moreop;
				bimm = first.num > second.num;
				break;
			case MOREE:
				qop = moreeop;
				bimm = first.num >= second.num;
				break;
			case NOTE:
				qop = noteop;
				bimm = first.num != second.num;
				break;
			case EQUAL:
				qop = equalop;
				bimm = first.num == second.num;
				break;
			default:
				break;
		}
		if (first.qt != IMM || second.qt != IMM) {
			addqua(qop, null, first, second);
			return;
		}
		else first.num = bimm;
	}
	second.qt = IMM;
	second.num = 0;
	addqua(noteop, null, first, second);
}
void ifstatment(unsigned int index) {
	if (get.nw != LBRA) {
		error(9);
		skip();
		return;
	}
	else get.nw = insymbol();
	condition(index);
	Quanode label;
	label.qt = LAB;
	label.num = labtab.size();
	labtab.push_back(0);
	addqua(bzop, null, label, null);
	if (get.nw != RBRA)
		error(18);
	else get.nw = insymbol();
	statment(index);
	labtab[label.num] = midcodes.size();
//	printf("this is if statment!\n");
}
void dostatment(unsigned int index) {
	Quanode label;
	label.qt = LAB;
	label.num = labtab.size();
	labtab.push_back(midcodes.size());
	statment(index);
	if (get.nw != WHILE)
		error(26);
	else get.nw = insymbol();
	if (get.nw != LBRA)
		error(9);
	else get.nw = insymbol();
	condition(index);
	if (get.nw != RBRA)
		error(18);
	else get.nw = insymbol();
	addqua(bnzop, null, label, null);
//	printf("this is do statment!\n");
}
void switchstatment(unsigned int index) {
	if (get.nw != LBRA) {
		error(9);
		skip();
		return;
	}
	else get.nw = insymbol();
	Quanode ereg;
	Type ty = expression(index, ereg);
	if (get.nw != RBRA)
		error(18);
	else get.nw = insymbol();
	if (get.nw != LLINE) {
		error(10);
		skip();
		return;
	}
	else get.nw = insymbol();
	memset(wordskip, 0, sizeof(wordskip));
	wordskip[CASE] = true;
	bool first = true;
	Quanode label, fin;
	label.qt = LAB;
	fin.qt = LAB;
	fin.num = labtab.size();
	labtab.push_back(0);
	while(true) {
		if (get.nw != CASE) {
			if (get.nw == RLINE) {
				if (first)
					error(13);
				break;
			}
			error(13);
		}
		else get.nw = insymbol();
		if (get.nw == ADD || get.nw == MINUS || get.nw == NUMBER || get.nw == ALPHA) {
			int sign = get.nw == MINUS? -1 : 1;
			if (get.nw == ADD || get.nw == MINUS) {
				get.nw = insymbol();
				if (get.nw != NUMBER) {
					error(5);
					skip();
					continue;
				}
			}
			Quanode tq;
			tq.qt = IMM;
			tq.num = get.nw == NUMBER?sign*get.number:get.ch;
			//add label
			if (!first)
				labtab[label.num] = midcodes.size();
			label.num = labtab.size();
			labtab.push_back(0);
			if((ty == INT && get.nw == NUMBER) || (ty == CHAR && get.nw == ALPHA)) {
				addqua(equalop, null, ereg, tq);
				addqua(bzop, null, label, null);
			}
			else error(23);
			get.nw = insymbol();
		}
		else {
			error(5);
			skip();
			continue;
		}
		if (get.nw == COLON)
			get.nw = insymbol();
		else error(14);
		statment(index);
		addqua(gotoop, null, fin, null);
		first = false;
	}
	labtab[label.num] = labtab[fin.num] = midcodes.size();
	wordskip[CASE] = false;
	if (get.nw != RLINE)
		error(19);
	else get.nw = insymbol();
//	printf("this is switch statment!\n");
}
void scanfstatment(unsigned int index) {
	if (get.nw != LBRA)
		error(9);
	else get.nw = insymbol();
	wordskip[SEMIC] = true;
	wordskip[RBRA] = true;
	Quanode scanq;
	scanq.qt = IMM;
	Quanode nv;
	nv.qt = CHA;
	while(true) {
		if (get.nw != IDENT) {
			error(16);
			skip();
			break;
		}
		else get.nw = insymbol();
		if (!search(index, get.identifier, nv.sl))
			error(11);
		else {
			if (nv.sl->second.ki == VAR) {
				scanq.num = nv.sl->second.ty == INT?5:12;
				addqua(scanop, nv, scanq, null);
			}
			else {
				error(23);
				skip();
				return;
			}
		}
		if (get.nw == RBRA)
			break;
		if (get.nw != COMMA) {
			error(6);
			skip();
			break;
		}
		else get.nw = insymbol();
	}
	wordskip[SEMIC] = false;
	wordskip[RBRA] = false;
	if (get.nw != RBRA)
		error(18);
	else get.nw = insymbol();
	if (get.nw != SEMIC)
		error(7);
	else get.nw = insymbol();
//	printf("this is scanf statment!\n");
}
void printfstatment(unsigned int index) {
	if (get.nw != LBRA)
		error(9);
	else get.nw = insymbol();
	Quanode ereg;
	Quanode prinq, strq;
	strq.qt = IMM;
	prinq.qt = IMM;
	Quanode nv;
	nv.qt = CHA;
	if (get.nw == STRING) {
		strq.num = strtab.size()-1;
		prinq.num = 4;
		get.nw = insymbol();
		addqua(printop, null, prinq, strq);
		if (get.nw == COMMA) {
			get.nw = insymbol();
			if(expression(index, ereg) == INT) {
				prinq.num = 1;
				addqua(printop, null, prinq, ereg);
			}
			else {
				prinq.num = 11;
				addqua(printop, null, prinq, ereg);
			}
		}
	}
	else{
		if(expression(index, ereg) == INT) {
			prinq.num = 1;
			addqua(printop, null, prinq, ereg);
		}
		else {
			prinq.num = 11;
			addqua(printop, null, prinq, ereg);
		}
	}
	if (get.nw != RBRA)
		error(18);
	else get.nw = insymbol();
	if (get.nw != SEMIC)
		error(7);
	else get.nw = insymbol();
//	printf("this is printf statment!\n");
}
void returnstatment(unsigned int index) {
	returned = true;
	auto sl = index2sys[index];
	if (get.nw == SEMIC) {
		get.nw = insymbol();
		if (sl->second.ty != VOID)
			error(17);
		else
			addqua(retop, null, null, null);
	//	printf("this is return statment!\n");
		return;
	}
	if (get.nw != LBRA)
		error(9);
	else get.nw = insymbol();
	Quanode ereg;
	if (sl->second.ty <= expression(index, ereg))
		addqua(retop, null, ereg, null);
	else error(23);
	if (get.nw != RBRA)
		error(18);
	else get.nw = insymbol();
	if (get.nw != SEMIC)
		error(7);
	else get.nw = insymbol();
//	printf("this is return statment!\n");
}
void callstatment(unsigned int index, const Quanode &nv, Quanode &midex) {
	if (get.nw == LBRA) { //callstatment
		get.nw = insymbol();
		Functab *ftab = nv.sl->second.exl.ftab;
		unsigned int ss = 0;
		if (get.nw != RBRA) {
			wordskip[RBRA] = true;
			while (true) {
				Quanode ereg;
				if(ss < ftab->size && expression(index, ereg) == ftab->para[ss++])
					addqua(pushop, null, ereg, null);
				else {
					error(15);
					skip();
					return;
				}
				if (get.nw == RBRA)
					break;
				if (get.nw != COMMA) {
					error(6);
					skip();
					return;
				}
				else get.nw = insymbol();
			}
		}
		if (nv.sl->second.ty == VOID)
			addqua(callop, null, nv, null);
		else {
			midex = reg_top();
			addqua(callop, midex, nv, null);
		}
		if (get.nw != RBRA)
			error(18);
		else get.nw = insymbol();
	}
//	printf("this is function call statment!\n");
	return;
}
void castatment(unsigned int index) {
	Quanode ereg, nv;
	nv.qt = CHA;
	Hashmap::iterator &sl = nv.sl;
	if (!search(index, get.identifier, sl)) {
		skip();
		return;
	}
	get.nw = insymbol();
	if (get.nw == LBRA) { //callstatment
		if (sl->second.ki != FUNCHEAD) {
			if (sl->second.ki != NOKIND)
				error(23);
			skip();
			return;
		}
		callstatment(index, nv, ereg);
	}
	else if (get.nw == ASSIG) {
		if (sl->second.ki != VAR) {
			if (sl->second.ki != NOKIND)
				error(23);
			skip();
			return;
		}
		get.nw = insymbol();
		expression(index, ereg);
		//if (sl->second.ty <= expression(index, ereg))
		addqua(assop, nv, ereg, null);
		//else
		//	error(24);
	}
	else if (get.nw == LARR) {
		if (sl->second.ki != ARRAY) {
			if (sl->second.ki != NOKIND)
				error(23);
			skip();
			return;
		}
		get.nw = insymbol();
		Quanode tar;
		expression(index, ereg);
		tar = ereg;
		//addqua(arrgetop, tar, nv, ereg);
		if (get.nw != RARR) {
			error(21);
			skip();
			return;
		}
		else get.nw = insymbol();
		if (get.nw != ASSIG) {
			error(4);
			skip();
			return;
		}
		else get.nw = insymbol();
		expression(index, ereg);
		//if (sl->second.ty <= expression(index, ereg))
		addqua(arrasop, nv, tar, ereg);
		//else error(24);
	}
	if (get.nw != SEMIC)
		error(7);
	else get.nw = insymbol();
//	printf("this is call or assign statment!\n");
}
//end of mess
Type expression(unsigned int index, Quanode &midex) {
	int sign = get.nw == MINUS?-1:1;
	if (get.nw == MINUS || get.nw == ADD)
		get.nw = insymbol();
	Type re = term(index, midex);
	Quanode tmpreg; 
	Quanode treg;
	Word lastop;
	if (sign == -1) {
		re = INT;
		if (midex.qt != IMM) {
			tmpreg = reg_top(); 
			Quanode minus;
			minus.qt = IMM;
			minus.num = -1;
			addqua(mulop, tmpreg, midex, minus);
			midex = tmpreg;
		}
		else midex.num = -midex.num;
	}
	while (get.nw == ADD || get.nw == MINUS) {
		re = INT;
		lastop = get.nw;
		get.nw = insymbol();
		term(index, treg);
		if (midex.qt != IMM || treg.qt != IMM)
			tmpreg = reg_top(); 
		else {
			midex.num = lastop == ADD?midex.num+treg.num:midex.num-treg.num;
			continue;
		}
		addqua(lastop == ADD?addop:subop,tmpreg,midex,treg);
		midex = tmpreg;
	}
	return re;
}
Type term(unsigned int index, Quanode &midex) {
	Type re = factor(index, midex);
	Quanode tmpreg;
	Quanode freg;
	Word lastop;
	while (get.nw == MULTI || get.nw == DIV) {
		re = INT;
		lastop = get.nw;
		get.nw = insymbol();
		factor(index, freg);
		if (midex.qt != IMM || freg.qt != IMM)
			tmpreg = reg_top(); 
		else {
			midex.num = lastop==MULTI?midex.num*freg.num:midex.num/freg.num;
			continue;
		}
		addqua(lastop == MULTI?mulop:divop,tmpreg,midex,freg);
		midex = tmpreg;
	}
	return re;
}
Type factor(unsigned int index, Quanode &midex) {
	if(get.nw == IDENT) {
		Quanode nv;
		nv.qt = CHA;
		Hashmap::iterator &sl = nv.sl;
		if (!search(index, get.identifier, sl)) {
			error(11);
		}
		get.nw = insymbol();
		if (get.nw == LARR) {
			if (sl->second.ki != ARRAY && sl->second.ki != NOKIND) {
				error(23);
			}
			get.nw = insymbol();
			const Quanode tmpreg = reg_top();
			Quanode ereg;
			expression(index, ereg);
			addqua(arrgetop, tmpreg, nv, ereg);
			if (get.nw != RARR) {
				error(21);
			}
			else get.nw = insymbol();
			midex = tmpreg;
		}
		else if(get.nw == LBRA) {
			if (sl->second.ki != FUNCHEAD || sl->second.ty == VOID) {
				if (sl->second.ki != NOKIND)
					error(15);
				skip();
				return INT;
			}
			else callstatment(index, nv, midex);
		}
		else if (sl->second.ki == VAR) {
			midex = nv;
		}
		else if (sl->second.ki == CON) {
			midex.qt = IMM;
			midex.num = sl->second.exl.cn;
		}
		return sl->second.ty;
	}
	else if (get.nw == NUMBER || get.nw == ALPHA || get.nw == ADD || get.nw == MINUS) {
		midex.qt = IMM;
		int sign = get.nw == MINUS? -1 : 1;
		if (get.nw == MINUS || get.nw == ADD) {
			get.nw = insymbol();
			if (get.nw != NUMBER) {
				error(5);
				return INT;
			}
		}
		if (get.nw == NUMBER) {
			midex.num = get.number*sign;
			get.nw = insymbol();
			return INT;
		}
		else {
			midex.num  = get.ch;
			get.nw = insymbol();
			return CHAR;
		}
	}
	else if (get.nw == LBRA) {
		get.nw = insymbol();
		Type re = expression(index, midex);
		if (get.nw != RBRA) {
				error(18);
		}
		else get.nw = insymbol();
		return re;
	}
	else {
		error(0);
		skip();
	}
	return INT;
}
void coutnode(const Quanode &q) {
	switch (q.qt) {
		case IMM:
			std::cout << q.num;
			break;
		case REG:
			std::cout << "tmp_" << q.num;
			break;
		case CHA:
			std::cout << q.sl->first;
			break;
		case LAB:
			std::cout << "l_" << q.num;
			break;
		case NOP:
			break;
	}
}
void couttynode(const Quanode &q) {
	switch (q.sl->second.ty) {
		case VOID:
			std::cout << "void ";
			break;
		case INT:
			std::cout << "int ";
			break;
		case CHAR:
			std::cout << "char ";
			break;
		default:
			break;
	}
	coutnode(q);
}

void addlabel() {
	unsigned int size = labtab.size();
	for(unsigned int i = 1; i < size; ++i)
		midcodes[labtab[i]].label.push_back(i);
}
void printmidcodes(std::vector<Qua> &midcodes) {
	for(auto it = midcodes.begin(); it != midcodes.end(); ++it) {
		if (it->label.size() == 0)
			std::cout << '\t';
		else
			for(auto i = it->label.begin(); i != it->label.end(); ++i)
				std::cout << "l_" << *i << "\t";
		if (it->op == addop || it->op == subop || it->op == mulop
		|| it->op == divop) {
			coutnode(it->re);
			std::cout << " = ";
			coutnode(it->nu1);
			switch(it->op) {
				case addop:
					std::cout << " + ";
					break;
				case subop:
					std::cout << " - ";
					break;
				case mulop:
					std::cout << " * ";
					break;
				case divop:
					std::cout << " / ";
					break;
				default:
					std::cout << " ? ";
			}
			coutnode(it->nu2);
			std::cout << std::endl;
			continue;
		}
		if(it->op == lessop || it->op == lesseop || it->op == moreop
			|| it->op == moreeop || it->op == noteop  || it->op == equalop) {
			coutnode(it->nu1);
			switch (it->op) {
				case lessop:
					std::cout << " < ";
					break;
				case lesseop:
					std::cout << " <= ";
					break;
				case moreop:
					std::cout << " > ";
					break;
				case moreeop:
					std::cout << " >= ";
					break;
				case noteop:
					std::cout << " != ";
					break;
				case equalop:
					std::cout << " == ";
					break;
				default:
					break;
			}
			coutnode(it->nu2);
			std::cout << std::endl;
			continue;
		}
		if (it->op == pushop || it->op == callop || it->op == retop
		||  it->op == gotoop || it->op == bnzop || it->op == bzop) {
			switch (it->op) {
				case pushop:
					std::cout << "push ";
					break;
				case callop:
					std::cout << "call ";
					break;
				case retop:
					std::cout << "ret ";
					break;
				case gotoop:
					std::cout << "goto ";
					break;
				case bnzop:
					std::cout << "bnz ";
					break;
				case bzop:
					std::cout << "bz ";
					break;
				default:
					break;
			}
			coutnode(it->nu1);
			std::cout << ' ';
			coutnode(it->re);
			std::cout << std::endl;
			continue;
		}
		if (it->op == paradec) {
			std::cout << "para ";
			++it;
		}
		char c;
		switch (it->op) {
			case funcdecop:
				couttynode(it->nu1);
				std::cout << "()";
				break;
			case constop:
				std::cout << "const ";
				couttynode(it->nu1);
				std::cout << " = ";
				if (it->nu1.sl->second.ty == CHAR) {
					c = it->nu1.sl->second.exl.cn;
					std::cout << '\'' << c << '\'';
				}
				else std::cout << it->nu1.sl->second.exl.cn;
				break;
			case varop:
				std::cout << "var ";
				couttynode(it->nu1);
				break;
			case assop:
				coutnode(it->re);
				std::cout << " = ";
				coutnode(it->nu1);
				break;
			case arrasop:
				coutnode(it->re);
				std::cout << "[";
				coutnode(it->nu1);
				std::cout << "] = ";
				coutnode(it->nu2);
				break;
			case arrgetop:
				coutnode(it->re);
				std::cout << " = ";
				coutnode(it->nu1);
				std::cout << "[";
				coutnode(it->nu2);
				std::cout << "]";
				break;
			case printop:
				std::cout << "print ";
				coutnode(it->nu1);
				std::cout << " ";
				if (it->nu1.num == 4) {
					std::cout << '"';
					std::cout << strtab[it->nu2.num];
					std::cout << '"';
				}
				else coutnode(it->nu2);
				break;
			case scanop:
				std::cout << "scan ";
				coutnode(it->nu1);
				std::cout << " ";
				coutnode(it->re);
				break;
			case arrdefop:
				couttynode(it->nu1);
				std::cout << "[";
				coutnode(it->nu2);
				std::cout << "]";
				break;
			case nop:
				std::cout << "nop";
				break;
			default:
				std::cout << "error!!!!!!!!!!!!!!!!";
		}
		std::cout << std::endl;
	}
}
void mapdata(unsigned int &it) {
	unsigned int gbcount = 0;
	fprintf(outputmips, ".data\ndata_begin:\n");
	while(midcodes[it].op != funcdecop) {
		Symbolline &sy = midcodes[it].nu1.sl->second;
		if (sy.ki == VAR) {
			fprintf(outputmips,".word 0\n");
			sy.addr = gbcount;
			gbcount += 4;
		}
		else if (sy.ki == ARRAY) {
			int arrl = sy.exl.arrlimit;
			fprintf(outputmips, ".word");
			fprintf(outputmips, " 0:%d\n", arrl);
			sy.addr = gbcount;
			gbcount += 4*arrl;
		}
		++it;
	}
	unsigned int s = strtab.size();
	for(unsigned int sti = 0; sti < s; ++sti)
		fprintf(outputmips, "str_%d: .asciiz \"%s\"\n", sti, strtab[sti].c_str());
}
void init() {
	fprintf(outputmips, ".text\n");
	fprintf(outputmips, "\tla $fp, data_begin\n");
	fprintf(outputmips, "\tjal main\n");
	fprintf(outputmips, "\tli $v0, 10\n");
	fprintf(outputmips, "\tsyscall\n");
}
inline void update(Quanode &q, unsigned int &addr, unsigned int bnum, bool re) {
	if (q.qt == CHA) {
		Symbolline &sy = q.sl->second;
		if (sy.ki == VAR) 
			++sy.count;
		if (sy.addr == 0xffffffff)  {
			if (sy.ki == VAR) {
				sy.addr = addr;
				addr += 4;
				usevar.push_back(q.sl);
			}
			else if (sy.ki == ARRAY) {
				int arrl = sy.exl.arrlimit;
				sy.addr = addr;
				addr += 4*arrl;
			}
		}
		if (sy.ki == VAR && bnum != 0
		&& la_map[bnum].la_defset.find(q.sl->first) == la_map[bnum].la_defset.end()
		&& la_map[bnum].la_useset.find(q.sl->first) == la_map[bnum].la_useset.end()) {
			if (re)
				la_map[bnum].la_defset.insert(q.sl->first);
			else
				la_map[bnum].la_useset.insert(q.sl->first);
		}
	}
	else if (q.qt == REG) {
		if (q.num == 0) return;
		Regnode &rn = regtab[q.num];
		rn.lastapp = bnum;
		if (rn.addr == 0xffffffff) {
			rn.addr = addr;
			addr += 4;
		}
		++rn.count;
	}
}
void mipstrans(Quanode &qnode, std::string reg, char t) {
	Symbolline *sy;
	switch (qnode.qt) {
		case REG:
			if (reg == regtab[qnode.num].areg) return;
			if (t == 'l')
				 fprintf(outputmips, "\tmove %s, $t%d\n", reg.c_str(), regtab[qnode.num].reg);
			else fprintf(outputmips, "\tmove $t%d, %s\n", regtab[qnode.num].reg, reg.c_str());
			break;
		case IMM:
			fprintf(outputmips, "\tli %s, %d\n", reg.c_str(), qnode.num);
			break;
		case CHA:
			sy = &(qnode.sl->second);
			if (reg == sy->areg) return;
			if (t == 'l') {
				fprintf(outputmips, "\tmove %s, %s\n", reg.c_str(), sy->areg.c_str());
			}
			else {
				fprintf(outputmips, "\tmove %s, %s\n", sy->areg.c_str(), reg.c_str());
			}
			break;
		default:
			break;
	}
}
void mipscond(Qua con, Qua jump, unsigned int line) {
	check(con.nu1, true, true, line);
	if (con.nu2.qt != IMM || con.nu2.num != 0)
		check(con.nu2, true, true, line);
	//mipstrans(con.nu1, "$t1", 'l');
	//mipstrans(con.nu2, "$t2", 'l');
	check(con.re, false, false, line);
	if(jump.op == bzop)
		switch (con.op) {
			case lessop:
				con.op = moreeop;
				break;
			case moreop:
				con.op = lesseop;
				break;
			case lesseop:
				con.op = moreop;
				break;
			case moreeop:
				con.op = lessop;
				break;
			case noteop:
				con.op = equalop;
				break;
			case equalop:
				con.op = noteop;
				break;
			default:
				break;
		}
	for (unsigned int it = 0; it < 10; ++it) {
		if (mtregs[it].qt == CHA) {
			trans(mtregs[it].sl, 's');
		}
	}
	switch (con.op) {
		case lessop:
			fprintf(outputmips, "\tblt");
			break;
		case moreop:
			fprintf(outputmips, "\tbgt");
			break;
		case lesseop:
			fprintf(outputmips, "\tble");
			break;
		case moreeop:
			fprintf(outputmips, "\tbge");
			break;
		case noteop:
			fprintf(outputmips, "\tbne");
			break;
		case equalop:
			fprintf(outputmips, "\tbeq");
			break;
		default:
			break;
	}
	if (con.nu2.qt == IMM && con.nu2.num == 0)
		fprintf(outputmips, "z %s, l_%d\n", str(con.nu1), jump.nu1.num);
	else
		fprintf(outputmips, " %s, %s, l_%d\n", str(con.nu1), str(con.nu2), jump.nu1.num);
	for (unsigned int it = 0; it < 10; ++it) {
		if (mtregs[it].qt == CHA) {
			mtregs[it].sl->second.areg = "";
		}
		mtregs[it] = null;
	}
	immtab.clear();
}
void op_3(Qua &q, unsigned int line) {
	check(q.nu1, true, true, line);
	check(q.nu2, true, true, line);
	//mipstrans(q.nu1, "$t1", 'l');
	//mipstrans(q.nu2, "$t2", 'l');
	check(q.re, false, false, line);
	switch(q.op) {
		case addop:
			fprintf(outputmips, "\tadd");
			break;
		case subop:
			fprintf(outputmips, "\tsub");
			break;
		case mulop:
			fprintf(outputmips, "\tmul");
			break;
		case divop:
			fprintf(outputmips, "\tdiv");
			break;
		default:
			break;
	}
	fprintf(outputmips, " %s, %s, %s\n", str(q.re), str(q.nu1), str(q.nu2));
	//mipstrans(q.re, "$t0", 's');
}
bool cmp(Hashmap::iterator x, Hashmap::iterator y) {
	return x->second.count > y->second.count;
}
inline void blockend() {
	//if ((i+1) == nmc.end() || (i+1)->blockstart != i->blockstart) {
	for (unsigned int it = 0; it < 10; ++it) {
		if (mtregs[it].qt == CHA) {
			trans(mtregs[it].sl, 's');
			mtregs[it].sl->second.areg = "";
		}
		mtregs[it] = null;
	}
	immtab.clear();
}

void comp(unsigned int &it) {
	Hashmap::iterator funcit = midcodes[it].nu1.sl;
	bool leaf = true;
	unsigned int i, addr = 4, pa = 0, ei;
	funcit->second.addr = 0;
	unsigned int pointer = funcit->second.exl.ftab->pointer;
	regtab.clear();
	immtab.clear();
	unsigned int firstline = 0;
	//split block
	for(i = it+1; i < size && midcodes[i].op != funcdecop; ++i) {
		if (midcodes[i].op == constop || midcodes[i].op == varop 
		||	midcodes[i].op == arrdefop|| midcodes[i].op == paradec)
			continue;
		if (firstline == 0) {
			firstline = i;
			midcodes[i].blockstart = 1;
		}
		if (!midcodes[i].label.empty())
			midcodes[i].blockstart = 1;
		if (midcodes[i].op == gotoop || midcodes[i].op == bnzop
		|| midcodes[i].op == bzop || midcodes[i].op == retop)
			midcodes[i+1].blockstart = 1;
	}
	ei = i;
	midcodes[i-1].blockstart = 1;
	//generate dag and opt
	nmc.clear();
	dag(firstline, ei);
	printmidcodes(nmc);
	usevar.clear();
	unsigned int countpara = 4;
	//update parameter
	for(i = it+1; i<ei; ++i) {
		if (midcodes[i].op == paradec) {
			++i;
			update(midcodes[i].nu1, pa, 0, false);
			if (countpara) {
				midcodes[i].nu1.sl->second.areg = "$ax";
				midcodes[i].nu1.sl->second.areg[2] = (4-countpara)+'0';
				--countpara;
			}
			//midcodes[i].nu1.sl->second.alloc = false;
			continue;
		}
		break;
	}
	//update normal var
	unsigned int bnum = 0;
	std::map<unsigned int, unsigned int> itmap;
	la_map.clear();
	la_map.push_back(la_null);
	for(auto i = nmc.begin(); i != nmc.end(); ++i) {
		if (i->blockstart) {
			i->blockstart = ++bnum;
			la_map.push_back(la_null);
		}
		else
			i->blockstart = bnum;
		if (!i->label.empty())
			for(auto li = i->label.begin(); li != i->label.end(); ++li)
				itmap[*li] = bnum; 
		if (i->op == callop)
			leaf = false;
		if (i->nu1.qt == CHA)
			update(i->nu1, addr, bnum, false);
		else 
			update(i->nu1, addr, i-nmc.begin(), false);
		if (i->nu2.qt == CHA)
			update(i->nu2, addr, bnum, false);
		else
			update(i->nu2, addr, i-nmc.begin(), false);
		if (i->re.qt == CHA)
			update(i->re, addr, bnum, true);
		else
			update(i->re, addr, i-nmc.begin(), true);
	}
	//sort var counts
	msregp = 0;
	for(unsigned int i = 0; i < 8; ++i) {
		msregs[i].clear();
		msregs[i].resize(bnum);
		for(unsigned int it = 0; it < bnum; ++it)
			msregs[i][it] = funcit;
	}
	std::vector<Hashmap::iterator> susevar(usevar);
	for(auto i = usevar.begin(); i != usevar.end(); ++i) {
		(*i);
	}
	std::sort(susevar.begin(), susevar.end(), cmp);
	for(auto i = usevar.begin(); i != usevar.end(); ++i) {
		(*i);
	}
	
	//life analysis build
	for(auto i = nmc.begin(); i != nmc.end(); ++i) {
		if (i->op == retop) {
			la_map[bnum].blockmap.push_back(i->blockstart);
			continue;
		}
		if (i->op == gotoop || i->op == bzop || i->op == bnzop) {
				la_map[itmap[i->nu1.num]].blockmap.push_back(i->blockstart);
			if (i->op != gotoop)
				la_map[(i+1)->blockstart].blockmap.push_back(i->blockstart);
			continue;
		}
		if ((i+1) != nmc.end() && (i+1)->blockstart != i->blockstart)
				la_map[(i+1)->blockstart].blockmap.push_back(i->blockstart);
	}
	for(unsigned int i = 1; i < bnum; ++i)
		la_map[i].la_inset = la_map[i].la_useset;
	bool anaend;
	do {
		anaend = false;
		for(unsigned int i = bnum-1; i > 0 ; --i) {
			la_update(i);
			for(auto it = la_map[i].blockmap.begin(); it != la_map[i].blockmap.end(); ++it)
				if(set_uni(*it, i)) anaend = true;
		}
	}while (anaend);
	// life analysis end
	// alloc regs
	for(auto i = susevar.begin(); i != susevar.end(); ++i) {
		if ((*i)->second.index == 0)
			continue;
		if ((*i)->second.areg != "") continue;
		bool fit = false;
		std::string first = (*i)->first;
		for(unsigned int ip = 0; ip < msregp; ++ip) {
			for(unsigned int ib = 1; ib < bnum; ++ib) {
				if (la_map[ib].la_defset.find(first) != la_map[ib].la_defset.end()
				||  la_map[ib].la_inset. find(first) != la_map[ib].la_inset.end()) {
					if (msregs[ip][ib] != funcit) {
						fit = false;
						break;
					}
					else fit = true;
				}
			}
			if (fit) {
				(*i)->second.areg = "$sx";
				(*i)->second.areg[2] = ip + '0';
				for(unsigned int ib = 1; ib < bnum; ++ib)
				if (la_map[ib].la_defset.find(first) != la_map[ib].la_defset.end()
				||  la_map[ib].la_inset. find(first) != la_map[ib].la_inset.end())
					msregs[ip][ib] = *i;
				break;
			}
		}
		if (fit) continue;
		if (msregp < 8) {
			(*i)->second.areg = "$sx";
			(*i)->second.areg[2] = msregp + '0';
			for(unsigned int ib = 1; ib < bnum; ++ib)
				if (la_map[ib].la_defset.find(first) != la_map[ib].la_defset.end()
				||  la_map[ib].la_inset. find(first) != la_map[ib].la_inset.end())
					msregs[msregp][ib] = *i;
			++msregp;
		}
	}
	addr -= 4;
	fprintf(outputmips, "%s:\n", funcit->first.c_str());
	if (!leaf) {
		fprintf(outputmips, "#leaf is false ,save ra\n");
		fprintf(outputmips, "\tsw $ra, ($sp)\n");
		fprintf(outputmips, "\tsubu $sp, $sp, 4\n");
		pa += 4;
	}
	pa += addr;
	for(i = it+1; i<ei; ++i) {
		if (midcodes[i].op == paradec) {
			++i;
			unsigned int &addr = midcodes[i].nu1.sl->second.addr;
			funcit->second.addr += 4;
			addr = pa - addr;
			continue;
		}
		break;
	}
	if (addr != 0) {
		fprintf(outputmips, "#alloc all var\n");
		fprintf(outputmips, "\tsubu $sp, $sp, %d\n", addr);
	}
	int calladdr = 0;
	//start genmipscode
	for(auto i = la_map[1].la_inset.begin(); i != la_map[1].la_inset.end(); ++i) {
		trans(str2it(pointer, *i), 'l');
	}
	for(auto i = nmc.begin(); i != nmc.end(); ++i) {
		if (!(i->label.size() == 0))
			for(auto li = i->label.begin(); li != i->label.end(); ++li) {
				fprintf(outputmips, "l_%d:\n", *li);
			}
		if (i->op == addop || i->op == subop || i->op == mulop
		|| i->op == divop) {
			fprintf(outputmips, "#ordinary calculate\n");
			op_3(*i, i-nmc.begin());
		}
		if(i->op == lessop || i->op == lesseop || i->op == moreop || i->op == moreeop || i->op == noteop  || i->op == equalop) {
			fprintf(outputmips, "#compare calculate\n");
			mipscond(*i, *(i+1), i-nmc.begin());
			++i;
			continue;
		}
		if (i->op == gotoop) {
			blockend();
			fprintf(outputmips, "\tj l_%d\n", i->nu1.num);
			continue;
		}
		if (i->op == retop) {
			fprintf(outputmips, "#return op\n");
			if (i->nu1.qt != IMM)
				check(i->nu1, true, false, i-nmc.begin());
			mipstrans(i->nu1, "$v0", 'l');
			blockend();
			fprintf(outputmips, "\tj %s_end\n", funcit->first.c_str());
			continue;
		}
		if (i->op == pushop) {
			fprintf(outputmips,"#push op\n");
			//mipstrans(i->nu1, "$t0", 'l');
			check(i->nu1, true, false, i-nmc.begin());
			fprintf(outputmips, "\tsw %s, %d($sp)\n", str(i->nu1), 0-calladdr);
			calladdr += 4;
		}
		if (i->op == callop) {
			fprintf(outputmips, "#call op\n");
			transallregs(i->blockstart, funcit, 's');
			if (calladdr != 0)
				fprintf(outputmips, "\tsubu $sp, $sp, %d\n", calladdr);
			fprintf(outputmips, "\tjal %s\n", i->nu1.sl->first.c_str());
			if (calladdr != 0)
				fprintf(outputmips, "\taddu $sp, $sp, %d\n", calladdr);
			transallregs(i->blockstart, funcit, 'l');
			calladdr = calladdr - i->nu1.sl->second.addr;
			check(i->re, false, false, i-nmc.begin());
			mipstrans(i->re, "$v0", 's');
		}
		switch (i->op) {
			case assop:
				fprintf(outputmips, "#assign op\n");
				if (i->nu1.qt != IMM)
					check(i->nu1, true, true, i-nmc.begin());
				//mipstrans(i->nu1, "$t1", 'l');
				check(i->re, false, false, i-nmc.begin());
				if (i->re.qt == CHA)
					mipstrans(i->nu1, i->re.sl->second.areg, 'l');
				else
					mipstrans(i->nu1, regtab[i->re.num].areg, 'l');
				//fprintf(outputmips, "\tmove %s, %s\n", str(i->re), str(i->nu1));
				break;
			case arrasop:
				fprintf(outputmips, "#array assign op\n");
				check(i->nu1, true, false, i-nmc.begin());
				//mipstrans(i->nu1, "$t1", 'l');
				fprintf(outputmips, "\tsll $gp, %s, 2\n", str(i->nu1));
				check(i->nu2, true, false, i-nmc.begin());
				//mipstrans(i->nu2, "$t2", 'l');
				if (i->re.sl->second.index != 0)
					fprintf(outputmips, "\taddu $gp, $gp, $sp\n");
				else
					fprintf(outputmips, "\taddu $gp, $gp, $fp\n");
				fprintf(outputmips, "\taddu $gp, $gp, %d\n", i->re.sl->second.addr);
				fprintf(outputmips, "\tsw %s, ($gp)\n", str(i->nu2));
				break;
			case arrgetop:
				fprintf(outputmips, "#array get op\n");
				check(i->nu2, true, false, i-nmc.begin());
				//mipstrans(i->nu2, "$t1", 'l');
				fprintf(outputmips, "\tsll $gp, %s, 2\n", str(i->nu2));
				if (i->nu1.sl->second.index != 0)
					fprintf(outputmips, "\taddu $gp, $gp, $sp\n");
				else
					fprintf(outputmips, "\taddu $gp, $gp, $fp\n");
				fprintf(outputmips, "\taddu $gp, $gp, %d\n", i->nu1.sl->second.addr);
				check(i->re, false, false, i-nmc.begin());
				fprintf(outputmips, "\tlw %s, ($gp)\n", str(i->re));
				//mipstrans(i->re, "$t0", 's');
				break;
			case printop:
				fprintf(outputmips, "#print op\n");
				//save $a0
				if (!usevar.empty() && usevar[0]->second.areg == "$a0")
					trans(usevar[0], 's');
				if (i->nu1.num == 4) {
					fprintf(outputmips, "\tla $a0, str_%d\n", i->nu2.num);
				}
				else {
					if (i->nu2.qt != IMM)
						check(i->nu2, true, false, i-nmc.begin());
					mipstrans(i->nu2, "$a0", 'l');
				}
				//else mipstrans(i->nu2, "$a0", 'l');
				fprintf(outputmips, "\tli $v0, %d\n", i->nu1.num);
				fprintf(outputmips, "\tsyscall\n");
				//load $a0
				if (!usevar.empty() && usevar[0]->second.areg == "$a0")
					trans(usevar[0], 'l');
				break;
			case scanop:
				fprintf(outputmips, "#scan op\n");
				fprintf(outputmips, "\tli $v0, %d\n", i->nu1.num);
				fprintf(outputmips, "\tsyscall\n");
				check(i->re, false, false, i-nmc.begin());
				mipstrans(i->re, "$v0", 's');
				break;
			default:
				break;
		}
		//call should save useing tmp
		if ((i+1) == nmc.end() || (i+1)->blockstart != i->blockstart)
			blockend();
	}
	for(auto i = systab[0].begin(); i != systab[0].end(); ++i)
		i->second.areg = "";
	fprintf(outputmips, "%s_end:\n", funcit->first.c_str());
	if (addr != 0)
		fprintf(outputmips, "\taddu $sp, $sp, %d\n", addr);
	if (!leaf) {
		fprintf(outputmips, "\taddu $sp, $sp, 4\n");
		fprintf(outputmips, "\tlw $ra, ($sp)\n");
	}
	fprintf(outputmips, "\tjr $ra\n");
	it = ei;
}
void genmips() {
	outputmips = fopen("test.s", "w");
	unsigned int it = 0;
	mapdata(it);
	init();
	for(unsigned int i = 0; i < 10; ++i)
		mtregs[i] = null;
	while(it < size)
		comp(it);
	fclose(outputmips);
}

int main(int argc, char **argv) {
	/*
	if (argc != 2) {
		fprintf(stderr, "you should enter your file path");
		return 0;
	}
	*/
	Hashmap em;
	Hashmap::iterator hiii;
	systab.push_back(em);
	index2sys.push_back(hiii);
	labtab.push_back(0);
	std::string path;
	std::cout << "enter your file path:";
	std::cin >> path;
	input = fopen(path.c_str(), "r");
	if (input == NULL) {
		fprintf(stderr, "cannot open the file!");
		return 0;
	}
	get.row = 1;
	get.column = 1;
	get.nw = insymbol();
	conststatment(0);
	varstatment(0);
	funcdec();
	/*
	int count = 0;
	}*/
	//printf("%d", midcodes.size());
	addlabel();
	//printmidcodes(midcodes);
	size = midcodes.size();
	genmips();
	fclose(input);
}
void dag(unsigned int beg, unsigned int end) {
	unsigned int i = beg;
	Qua glabel;
	glabel.op = nop;
	glabel.re = glabel.nu1 = glabel.nu2 = null;
	glabel.blockstart = 1;
	while (i != end) {
		glabel.label = midcodes[i].label;
		nmc.push_back(glabel);
		do {
			if (midcodes[i].op == addop || midcodes[i].op == subop
			||	midcodes[i].op == mulop || midcodes[i].op == divop
			||	midcodes[i].op == assop)
				builddag(midcodes[i]);
			else outsingledag(midcodes[i]);
		}while(++i < end && !midcodes[i].blockstart);
		outdag();
		cleardag();
	}
}
inline unsigned int newdagnode(dagnode &dn, Quaop op, unsigned int q) {
	dn.out = op == nop;
	dn.nv = op == nop?1:0;
	dn.dg = 0;
	dn.op = op;
	dn.q = q;
	dagmap.push_back(dn);
	return dagmap.size()-1;
}
unsigned int getdagnode(Quanode &qn) {
	for(auto it = nodemap.begin(); it != nodemap.end(); ++it) {
		if (it->q == qn)
			return it->fs == invalid ? it->ns : it->fs;
	}
	listnode nl;
	nl.q = qn;
	nl.fs = invalid;
	dagnode nd;
	nd.le = nd.ri = invalid;
	nl.ns = newdagnode(nd, nop, nodemap.size());
	nodemap.push_back(nl);
	return nl.ns;
}
Lptr getlistnode(Quanode &qn) {
	for(auto it = nodemap.begin(); it != nodemap.end(); ++it) {
		if (it->q == qn)
			return it;
	}
	listnode nl;
	nl.q = qn;
	nl.fs = nl.ns = invalid;
	nodemap.push_back(nl);
	return --nodemap.end();
}
inline bool dagequal(unsigned int d1, unsigned int d2, unsigned int d3, unsigned int d4) {
	return ((d1 == d3 && d2 == d4) || (d1 == d4 && d2 == d3));
}
inline void mkvalid(unsigned int n, unsigned int q) {
	if (n != invalid) {
		if (dagmap[n].q == q) {
			unsigned int nq = invalid;
			for(auto it = nodemap.begin(); it != nodemap.end(); ++it)
				if (it->fs == n && (unsigned int)(it - nodemap.begin()) != q) {
					nq = it - nodemap.begin();
					if (it->q.qt == CHA) break;
				}
			dagmap[n].q = nq;
		}
		if (dagmap[n].op == nop)
			--dagmap[n].dg;
		else {
			--dagmap[dagmap[n].le].dg;
			--dagmap[dagmap[n].ri].dg;
		}
	}
}
void builddag(Qua &mc) {
	if (mc.op == assop) {
		unsigned int ln = getdagnode(mc.nu1);
		Lptr rln = getlistnode(mc.re);
		unsigned int urln = rln - nodemap.begin();
		mkvalid(rln->fs, urln);
		rln->fs = ln;
		if (dagmap[ln].op != nop && nodemap[dagmap[ln].q].q.qt == REG && rln->q.qt == CHA) {
			dagmap[ln].q = urln;
			dagmap[ln].out = false;
		}
		++dagmap[ln].dg;
		return;
	}
	unsigned int ln1, ln2;
	Lptr rln;
	ln1 = getdagnode(mc.nu1);
	ln2 = getdagnode(mc.nu2);
	rln = getlistnode(mc.re);
	unsigned int urln = rln - nodemap.begin();
	for(auto it = dagmap.begin(); it != dagmap.end(); ++it)
		if (it->op == mc.op && dagequal(it->le, it->ri, ln1, ln2)) {
			mkvalid(rln->fs, urln);
			rln->fs = it-dagmap.begin();
			if (it->q == invalid || (nodemap[it->q].q.qt == REG && rln->q.qt == CHA)) {
				it->q = urln;
				it->out = false;
			}
			if (dagmap[rln->fs].op == nop)
				++dagmap[rln->fs].dg;
			else {
				++dagmap[dagmap[rln->fs].le].dg;
				++dagmap[dagmap[rln->fs].ri].dg;
			}
			return;
		}
	dagnode fdn;
	fdn.le = ln1;
	fdn.ri = ln2;
	mkvalid(rln->fs, urln);
	rln->fs = newdagnode(fdn, mc.op, urln);
	++dagmap[dagmap[rln->fs].le].dg;
	++dagmap[dagmap[rln->fs].ri].dg;
}
void outdag() {
	bool notend;
	do{
		notend = false;
		for(auto it = nodemap.begin(); it != nodemap.end(); ++it)
			if (it->fs != invalid) {
				notend = true;
				if (dagmap[it->fs].op != nop && (!dagmap[dagmap[it->fs].le].out 
				||	!dagmap[dagmap[it->fs].ri].out))
					continue;
				if (it->ns != invalid) {
					if(dagmap[it->ns].nv == 1 && dagmap[it->ns].dg > 0) {
						unsigned int selfuse = 0;
						if (dagmap[it->fs].le == it->ns)
							++selfuse;
						if (dagmap[it->fs].ri == it->ns)
							++selfuse;
						if (dagmap[it->ns].dg > selfuse)
							continue;
					}
					--dagmap[it->ns].nv;
					if (nodemap[dagmap[it->ns].q].q == it->q) {
						unsigned int nq = invalid;
						for(auto i = nodemap.begin(); i != nodemap.end(); ++i)
							if (i->ns == it->ns && i != it) {
								nq = i - nodemap.begin();
								if(i->q.qt == CHA)
									break;
							}
						if (nq != invalid && nodemap[nq].q.qt == REG && dagmap[it->ns].op != nop)
							dagmap[it->ns].out = false;
						if (nq != invalid)
							dagmap[it->ns].q = nq;
					}
				}
				it->ns = it->fs;
				it->fs = invalid;
				if (dagmap[it->ns].op == nop)
					--dagmap[it->ns].dg;
				else {
					--dagmap[dagmap[it->ns].le].dg;
					--dagmap[dagmap[it->ns].ri].dg;
				}
				if(!dagmap[it->ns].out) {
					Qua mc;
					mc.op = dagmap[it->ns].op;
					mc.re = nodemap[dagmap[it->ns].q].q;
					mc.nu1 = nodemap[dagmap[dagmap[it->ns].le].q].q;
					mc.nu2 = nodemap[dagmap[dagmap[it->ns].ri].q].q;
					mc.blockstart = 0;
					nmc.push_back(mc);
					dagmap[it->ns].out = true;
					++dagmap[it->ns].nv;
				}
				if (dagmap[it->ns].q != (unsigned int)(it - nodemap.begin()) && it->q.qt == CHA) {
					Qua mc;
					mc.op = assop;
					mc.re = it->q;
					mc.nu1 = nodemap[dagmap[it->ns].q].q;
					mc.nu2 = null;
					mc.blockstart = 0;
					nmc.push_back(mc);
					++dagmap[it->ns].nv;
				}
				break;
			}
	}
	while (notend);
}
inline void var2dag(Quanode &qd) {
	for(auto it = nodemap.begin(); it != nodemap.end(); ++it)
		if (it->q == qd) {
			qd = nodemap[dagmap[it->ns].q].q;
			return;
		}
}
void flush(Quanode &qn) {
	for(auto it = nodemap.begin(); it != nodemap.end(); ++it)
		if (it->q == qn) {
			if (nodemap[dagmap[it->ns].q].q == qn) {
				unsigned int nq = invalid;
				for(auto i = nodemap.begin(); i != nodemap.end(); ++i)
					if (i->ns == it->ns && i != it ) {
						nq = i - nodemap.begin();
						if(i->q.qt == CHA)
							break;
					}
					if (nq != invalid && nodemap[nq].q.qt == REG && dagmap[it->ns].op != nop)
							dagmap[it->ns].out = false;
				if (nq != invalid)
					dagmap[it->ns].q = nq;
			}
			--dagmap[it->ns].nv;
			dagnode nd;
			nd.le = nd.ri = invalid;
			it->ns = newdagnode(nd, nop, it-nodemap.begin());
			return;
		}
}

Qua dagch(Qua &mc) {
	//scanop, arrasop, arrgetop, callop
	Qua result = mc;
	result.blockstart = 0;
	result.label.clear();
	switch(mc.op) {
		case printop:
			var2dag(result.nu2);
			return result;
		case scanop:
			flush(result.re);
			return result;
		case arrgetop:
			flush(result.re);
			var2dag(result.nu2);
			return result;
		case callop:
			for(auto it = nodemap.begin(); it != nodemap.end(); ++it)
				if (it->q.qt == CHA && it->q.sl->second.index == 0) {
					if (nodemap[dagmap[it->ns].q].q == it->q) {
						unsigned int nq = invalid;
						for(auto i = nodemap.begin(); i != nodemap.end(); ++i)
							if (i->ns == it->ns && i != it ) {
								nq = i - nodemap.begin();
								if (i->q.qt == CHA)
									break;
							}
						if (nq != invalid && nodemap[nq].q.qt == REG && dagmap[it->ns].op != nop)
							dagmap[it->ns].out = false;
						if (nq != invalid)
							dagmap[it->ns].q = nq;
					}
					--dagmap[it->ns].nv;
					dagnode nd;
					nd.le = nd.ri = invalid;
					it->ns = newdagnode(nd, nop, it-nodemap.begin());
				}
			return result;
		default:
			var2dag(result.nu1);
			var2dag(result.nu2);
			return result;
	}
}
void outsingledag(Qua &mc) {
	outdag();
	if (mc.op != nop)
		nmc.push_back(dagch(mc));
}
void cleardag() {
	nodemap.clear();
	dagmap.clear();
}
inline void la_update(unsigned int x) {
	for(auto i = la_map[x].la_outset.begin(); i != la_map[x].la_outset.end(); ++i)
		if (la_map[x].la_defset.find(*i) == la_map[x].la_defset.end())
			la_map[x].la_inset.insert(*i);
}
inline bool set_uni(unsigned int tar, unsigned int src) {
	bool re = false;
	for(auto i = la_map[src].la_inset.begin(); i != la_map[src].la_inset.end(); ++i)
		if (la_map[tar].la_outset.find(*i) == la_map[tar].la_outset.end()) {
			re = true;
			la_map[tar].la_outset.insert(*i);
		}
	return re;
}
inline Hashmap::iterator str2it(unsigned int pointer, std::string str) {
	Hashmap::iterator it;
	if((it = systab[pointer].find(str)) == systab[pointer].end())
		it = systab[0].find(str);
	return it;
}
void trans(Hashmap::iterator x, char t) {
	if (x->second.areg != "") {
		char c = x->second.ty == INT ? 'w' : 'b';
		char s = x->second.index == 0 ? 'f' : 's';
		fprintf(outputmips, "\t%c%c %s, %d($%cp)\n", t, c,
		x->second.areg.c_str(), x->second.addr, s);
	}
}
const char *str(Quanode &q) {
	switch (q.qt) {
		case REG:
			return regtab[q.num].areg.c_str();
		case IMM:
			return immtab[q.num].c_str();
		case CHA:
			return q.sl->second.areg.c_str();
		default:
			break;
	}
	return 0;
}
unsigned int getfreereg(Quanode q, unsigned int line) {
	unsigned int re = 10;
	for(unsigned int i = 0; i < 10; ++i) {
		if (locked[i]) continue;
		if (mtregs[i] == null) {
			re = i;
			break;
		}
		if (mtregs[i].qt == REG && line > regtab[q.num].lastapp) {
			re = i;
			break;
		}
		if (mtregs[i].qt == IMM || mtregs[i].qt == CHA) {
			re = i;
			continue;
		}
		if (re == 10) {
			re = i;
			continue;
		}
		if (mtregs[i].qt == mtregs[re].qt && regtab[mtregs[i].num].lastapp > regtab[mtregs[re].num].lastapp)
			re = i;
	}
	switch (mtregs[re].qt) {
		case CHA:
			trans(mtregs[re].sl, 's');
			mtregs[re].sl->second.areg = "";
			break;
		case IMM:
			immtab.erase(mtregs[re].num);
			break;
		case REG:
			fprintf(outputmips, "\tsw %s, %d($sp)\n", regtab[mtregs[re].num].areg.c_str(), regtab[mtregs[re].num].addr);
			regtab[mtregs[re].num].areg = "";
			regtab[mtregs[re].num].reg = 10;
			break;
		default:
			break;
	}
	mtregs[re] = q;
	return re;
}
void check(Quanode &q, bool sload, bool nlock, unsigned int line) {
	unsigned int t = 10;
	switch (q.qt) {
		case CHA:
			if (q.sl->second.areg != "")
				break;
			t = getfreereg(q, line);
			q.sl->second.areg = "$tx";
			q.sl->second.areg[2] = t + '0';
			if (sload)
				trans(q.sl, 'l');
			break;
		case IMM:
			if (immtab.find(q.num) != immtab.end()) {
				//std::cout << immtab[q.num];
				t = immtab[q.num][2] - '0';
				break;
			}
			t = getfreereg(q, line);
			immtab[q.num] = "$tx";
			immtab[q.num][2] = t + '0';
			if (sload)
				fprintf(outputmips, "\tli %s, %d\n", immtab[q.num].c_str(), q.num);
			break;
		case REG:
			if (regtab[q.num].areg != "") {
				t = regtab[q.num].areg[2] - '0';
				break;
			}
			t = getfreereg(q, line);
			regtab[q.num].areg = "$tx";
			regtab[q.num].areg[2] = t + '0';
			regtab[q.num].reg = t;
			if (sload)
				fprintf(outputmips, "\tlw %s, %d($sp)\n", regtab[q.num].areg.c_str(), regtab[q.num].addr);
			break;
		default:
			break;
	}
	if (nlock)
		locked[t] = true;
	else memset(locked, 0, sizeof(locked));
}
void transallregs(unsigned int bnum, Hashmap::iterator nu, char t) {
	for(auto i = usevar.begin(); i != usevar.end(); ++i) {
		if ((*i)->second.areg != "" && (*i)->second.areg[1] == 'a')
			trans(*i, t);
		else break;
	}
	for(unsigned int i = 0; i < 8; ++i)
		if (msregs[i][bnum] != nu)
			trans(msregs[i][bnum], t);
	for(unsigned int i = 0; i < 10; ++i) {
		//if (mtregs[i] == null) continue;
			switch (mtregs[i].qt) {
				case CHA:
					trans(mtregs[i].sl, t);
					break;
				case REG:
					fprintf(outputmips, "\t%cw %s, %d($sp)\n", t, regtab[mtregs[i].num].areg.c_str(), regtab[mtregs[i].num].addr);
					break;
				case IMM:
					if (t == 'l') {
						//fprintf(outputmips, "\tli %s, %d\n", immtab[mtregs[i].num].c_str(), mtregs[i].num);
						immtab.erase(mtregs[i].num);
						mtregs[i] = null;
					}
				default:
					break;
			}
	}
}
