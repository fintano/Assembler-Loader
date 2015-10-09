#include "linkedlist.h"

/* list �ʱ�ȭ �Լ� */
void listinit(Linkedlist *plist){
	plist -> head = (Node*)malloc(sizeof(Node));
	plist -> tail = NULL;
	plist -> head -> next = NULL;
	plist -> numOfData = 0;
}
/* list �Է� */

void listinsert(Linkedlist *plist, Data data){
	Node *newnode = (Node*)malloc(sizeof(Node));
	newnode -> data = (Data)malloc(sizeof(strlen(data)+1));
	strcpy(newnode -> data,data);
	newnode -> next = NULL;	

	if(plist->head->next == NULL){
		plist->head->next = newnode;
	}
	else{
		plist->tail->next = newnode;
	}

	plist->tail = newnode;
	plist -> numOfData++;
}

/* value ���� insert �ϱ� ���� �Լ� */

void listinsertmore(Linkedlist *plist, Data value){
	plist->tail->value = (Data)malloc(sizeof(strlen(value)+1));
	strcpy(plist->tail->value,value);
}
/*format ���� insert �ϱ� ���� �Լ�*/

void listinsertformat(Linkedlist *plist, Data format){
	
	plist->tail->format = (Data)malloc(sizeof(strlen(format)+1));
	strcpy(plist->tail->format,format);
}

/* ����Ʈ ù��° search */

int LFirst(Linkedlist *list, Data *data){

	if(list->head->next == NULL)
		return 0;
	
	list->cur = list->head->next;

	*data = list->cur->data;
	return 1;
}
/* ����Ʈ ���� �� search */
int LNext(Linkedlist *list, Data *data){
	
	if(list->cur->next == NULL)
		return 0;
	
	list->cur = list->cur->next;
	*data = list->cur->data;
	return 1;
}

/* value�� return */

char* findValue(Linkedlist *list){
	return list->cur->value;
}
char* findFormat(Linkedlist *list){
	return list->cur->format;
}
/* data�� �ش��ϴ� value ���� �ִ��� �������� ��ȯ */

int LPeek(Linkedlist *list, Data data){

	list->cur = list->head->next;
	while(list->cur != NULL){
		if( strcmp(list->cur->value,data) == 0 )
			return 1;
		list->cur = list->cur->next;
	}
	return 0;
}

/* list�� �ִ� ���� ���� */
void LRemove(Linkedlist *list){

	Node *node;
	list->cur = list->head->next;
	while(list->cur != NULL){
		node = list->cur;
		list->cur = list->cur->next;
		free(node);
		list->numOfData--;
	}
	list->head->next = NULL;
}
 
