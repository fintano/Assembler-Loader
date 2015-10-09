#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef char* Data;

typedef struct _node{
	Data data,value,format; // opcode, mnemonic 둘다 넣기 위한 두개의 변수
	struct _node *next;
}Node;

typedef struct _linkedlist{
	Node *head;
	Node *tail;
	Node *cur;
	int numOfData;
}Linkedlist;

void listinit(Linkedlist *list);
void listinsert(Linkedlist *list, Data data);
void listinsertformat(Linkedlist *list, Data data);
void listinsertmore(Linkedlist *list,Data value);
char* findFormat(Linkedlist *list);
char* findValue(Linkedlist *list);
int LFirst(Linkedlist *list, Data *data);
int LNext(Linkedlist *list, Data *data);
int LPeek(Linkedlist *list, Data data);
void LRemove(Linkedlist *list);

#endif
