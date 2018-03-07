#ifndef _COMP_H_
#define _COMP_H_
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_set>
#include <list>
#include <unordered_map>
#include <map>
#include <vector>
#include <cctype>
#include <utility>
enum Kind {CON, VAR, FUNCHEAD, ARRAY, NOKIND};
enum Type {INT, CHAR, VOID, NOTYPE};
enum Word { ADD, MINUS, MULTI, DIV, LESS, LESSE, MAIN,
			MORE, MOREE, NOTE, EQUAL, NUMBER, ALPHA, STRING,
			CONST, SEMIC, COMMA, TYPEI, TYPEC, ASSIG, LLINE,
			RLINE, TYPEV, LARR, RARR, IF, DO, WHILE, CASE,
			SWITCH, COLON, RETURN, PRINTF, SCANF, LBRA, RBRA, IDENT, ERROR, WORDNU};
enum Quaop {addop, subop, mulop, divop, lessop, lesseop, moreop, moreeop, noteop, equalop, funcdecop, paradec, pushop, callop, retop, varop, constop, assop, gotoop, bnzop, bzop, arrasop, arrgetop, printop, scanop, arrdefop, nop};

struct Functab {
	Type *para;
	unsigned int pointer, size;
};
union Exline {
	Functab *ftab;
	int cn;
	int arrlimit;
};
struct Symbolline {
	//std::string name;
	Kind ki;
	Type ty;
	Exline exl;
	//genmips
	unsigned int index;
	unsigned int addr;
	unsigned int count;
	//unsigned int reg;
	//bool first, alloc;
	std::string areg;
};
struct Regnode {
	unsigned int addr = 0xffffffff;
	unsigned int count = 0;
	unsigned int reg = 10;
	unsigned int lastapp;
	std::string areg;
};
std::map<unsigned int, std::string> immtab;
struct {
	std::string identifier;
	unsigned int number;
	char ch;
	int column, row;
	Word nw;
} get;
typedef std::unordered_map<std::string, Symbolline> Hashmap;
enum Quatype {REG, IMM, CHA, LAB, NOP};
struct Quanode {
	Quatype qt;
	int num;
	Hashmap::iterator sl;
};
Quanode null = {NOP};
//const Quanode retreg = {REG, 0};
struct Qua {
	Quaop op;
	Quanode re, nu1, nu2;
	std::vector<unsigned int> label;
	unsigned int blockstart;
};
unsigned int size;
bool errored, returned;
std::unordered_map<unsigned int, Regnode> regtab;
std::vector<std::string> strtab;
std::vector<Hashmap::iterator> index2sys, usevar, susevar;
std::vector<unsigned int> labtab;//label table
std::vector<Hashmap> systab;
std::vector<Qua> midcodes;
FILE *input, *outputmips;
bool wordskip[WORDNU];
Quanode mtregs[10];
bool locked[10];
std::vector<Hashmap::iterator> msregs[8];
unsigned int msregp;
//dag use
//life analysis here
struct la_node {
	std::list<unsigned int> blockmap;
	std::unordered_set<std::string> la_useset;
	std::unordered_set<std::string> la_defset;
	std::unordered_set<std::string> la_inset;
	std::unordered_set<std::string> la_outset;
	bool inqueue = false;
} la_null;
std::vector<la_node> la_map;
//std::vector<Hashmap::iterator> usevar;
//
std::vector<Qua> nmc;//new midcodes
const unsigned int invalid = 0xffffffff;
struct dagnode{
	bool out;
	unsigned int nv, dg;
	unsigned int le, ri;
	Quaop op;
	unsigned int q;
};
struct listnode{
	Quanode q;
	unsigned int ns, fs;//now state future state
};
typedef std::vector<listnode> Nodemap;
Nodemap nodemap;
typedef Nodemap::iterator Lptr;
std::vector<dagnode> dagmap;
//dag end
const char *errormsg[] = {
	"unexpected word",
	"unknown or illegal symbol",
	"expected type int or char",
	"it should be character",
	"expected assign",
	"it should be number",
	"expected ','",
	"expected ';'",
	"it should be unsigned number",
	"expected '('",
	"ecpected '{'",
	"undefined identifier",
	"expected operators",
	"expected case",
	"expected ':'",
	"illegal function call",
	"expected identifier",
	"it should have a return value at the end of the function",
	"expected ')'",
	"expected '}'",
	"redefined identifier",
	"expected ']'",
	"it should have a return value",
	"cannot match its type",
	"cannot match array type",
	"expected 'void main'",
	"expected 'while'",
	"expected 'case'",
	"expected '='"};
Word insymbol();
void addqua(Quaop qop,const Quanode &re,const Quanode &nu1,const Quanode &nu2);
const Quanode reg_top();
void addsys(unsigned int index, std::string name, Symbolline &data);
void conststatment(unsigned int index);
void varstatment(unsigned int index);
void funcdec();
void funchead(Type ty, std::string name);
bool statments(unsigned int index);
Word statment(unsigned int index);
void condition(unsigned int index);
void ifstatment(unsigned int index);
void dostatment(unsigned int index);
void switchstatment(unsigned int index);
void scanfstatment(unsigned int index);
void printfstatment(unsigned int index);
void returnstatment(unsigned int index);
void castatment(unsigned int index);
Type expression(unsigned int index, Quanode &midex);
Type term(unsigned int index, Quanode &midex);
Type factor(unsigned int index, Quanode &midex);
void genmips();
void dag(unsigned int beg, unsigned int end);
void builddag(Qua &mc);
void outsingledag(Qua &mc);
void outdag();
void cleardag();
void la_update(unsigned int x);
bool set_uni(unsigned int tar, unsigned int src);
Hashmap::iterator str2it(unsigned int pointer, std::string str);
void check(Quanode &q, bool sload, bool nlock, unsigned int line);
void transallregs(unsigned int bnum, Hashmap::iterator nu, char t);
//void loadallregs(unsigned int bnum);
const char *str(Quanode &q);
void trans(Hashmap::iterator x, char t);
#endif
