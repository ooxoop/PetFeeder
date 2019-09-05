#ifndef __LINKLIST_LIST_H
#define __LINKLIST_LIST_H

#include <stdio.h>
#include <stdlib.h>  



typedef struct LinkNode{
	int timecount;
	int weight;
	struct LinkNode *pNext;
}node, *pNode;


pNode list_saveDiet(char*);
void list_free(pNode pHead);

extern pNode pList;

//extern node pNode;

#endif
