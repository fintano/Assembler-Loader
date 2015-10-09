#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "linkedlist.h"
#include "UsefulHeap.h"
#include <math.h>
#include <ctype.h>

#define MAX_HASH_SIZE 20

typedef struct _value{
	char value[10]; // ParameterÀÇ String °ª
	int exist; // value°¡ µé¾î¿Ô´ÂÁö °Ë»ç
}value;

typedef enum {A,R} Type;

int manager(char* order, Linkedlist *list, Linkedlist *objName);
int takeToken(char* order, value* para1, value* para2, value* para3, Linkedlist *objName);
int opcode(value* para1);
int mnemonic(value* para1);
void opcodeList(void);
void mnemonicList(void);
void makeTable(void);

int dump(value* para1, value* para2);
int edit(value* para1, value* para2);
int fill(value* para1, value* para2, value* para3);

void reset(void);
void help(void);
void history(char *order, Linkedlist *list);
void dir(void);

int StringToHex(char* string);
int MneToOp(char* str);
int OpToMne(char* op);
char* copy(int start, int end, char* array);
void type(value* filename);

int Pass1(char* filename);
int Pass2(char* filename);
Data getAddr(char* operand,Linkedlist list);
Data getData(char* operand,Linkedlist list);
Data registerTr(char* mnemonic);
int assemble(value* filename);
void symbol(void);

void putLiteral(Linkedlist Llist,int *loc_int);
void printLiteral(Linkedlist Llist,FILE *printLine, FILE *getLine, char *line2, char* opcode_flag, char *mnemonic);
int EQU(char* operand,int *expression);
void ORG(char *operand, int *ORGFlag, int *loc_int, int *originalAddr);

int isStringDouble(char *s);
void Trecord(char *opcode_flag, char *mnemonic, char *fullOpcode, char* line2);
Data sendByteString(char *operand);

int Pass2_LinkingLoader_H(char* record, int* length,Linkedlist *RSymbol);
int Pass2_LinkingLoader_R(char *record, Linkedlist *RSymbol);
int Pass2_LinkingLoader_T(char *record);
int Pass2_LinkingLoader_M(char *record, Linkedlist *RSymbol);
int Pass2_LinkingLoader(Linkedlist *objName);

int subAndBorrowCheck(int var, int var2, int borrow, int *sum);
int addAndCarryCheck(int var, int var2, int carry, int *sum);

int Pass1_LinkingLoader(Linkedlist *objName);
int Pass1_LinkingLoader_H(char* record, int* length);
int Pass1_LinkingLoader_D(char* record);

int loader(Linkedlist *objName);
int progaddr(value *para1);
void initialProgram(void);
void RExcess16(int *q, int *r);

int progaddress; // starting addr
int csaddress; // control section addr
Type symbolType;
unsigned char memory[16][65536];
 int strcnt;
 int dumpCnt;
 Linkedlist external_table[20];
 Linkedlist opcode_table[20];
 Linkedlist mnemonic_table[20];
 Linkedlist symbol_table[20];
 Linkedlist literal_table[20];
 int length;
char record[70];
FILE *fp4;

#endif
