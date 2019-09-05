#include <stdio.h>
#include <string.h>  
#include <stdlib.h>  
#include "linked_list.h"

#define chartonumber(x) 	(x-'0')

char* list_split(char*);

int data = 0;
char dataStr[16];

//增加元素
pNode list_add(pNode pHead, int c, int w){
	pNode pNew = (pNode)malloc(sizeof(node));
	
	pNew->timecount = c;
	pNew->weight = w;
	if(pHead == NULL){
		pHead = pNew;
		pNew->pNext = pHead;
	}else{
		pNode p = pHead;
		while(p->pNext != pHead){
			p = p->pNext;
		}
		p->pNext = pNew;
		pNew->pNext = pHead;
	}
	return pHead;
}

//遍历链表
void list_readAll(pNode pHead){
	printf("\r\n[List]遍历链表\r\n");
	if(pHead == NULL){
		return;
	}else if(pHead->pNext == pHead){
		printf("%d-->%d",pHead->timecount, pHead->weight); 
	}else{
		pNode p = pHead;
		while(p->pNext != pHead){
			printf("\r\n%d-->%d", p->timecount, p->weight);
			p = p->pNext;
		}
		printf("\r\n%d-->%d", p->timecount, p->weight);
		p = p->pNext;
		printf("\r\n%d-->%d(reload)\r\n", p->timecount, p->weight);
	}
}

//释放列表空间
void list_free(pNode pHead){
	pNode p = NULL;
	
	printf("\r\n[List]开始清空链表\r\n");
	while(pHead != NULL){
		if(pHead == NULL){
			return;
		}else if(pHead->pNext == pHead){
			free(pHead);
			pHead = NULL;
		}else{
			p = pHead->pNext->pNext;
			free(pHead->pNext);
			pHead->pNext = p;
		}
	}
	printf("\r\n[List]已清空链表\r\n");
}

//解析字符串并存入链表
pNode list_saveDiet(char* str){
	pNode phead = NULL;
	char *p;
	int c=0, w=0;
	
	p = list_split(str);
	c = data;
	while(p){
		p = list_split(p);
		w = data;
		phead = list_add(phead, c, w);
		if(!p)
			break;
		p = list_split(p);
		c = data;
	}
	list_readAll(phead);
	return phead;
}

//字符串分割
char* list_split(char* str){
	int i;
	memset(dataStr, 0, sizeof(dataStr));
	for(i=0; i<strlen(str); i++){
		if(str[i] == '.'){
			data = atof(dataStr);
			printf("\r\n[List]get number(%d)--%s\r\n", data, (str+i+1));
			return (str + i + 1);
		}
		dataStr[i] = str[i]; 
	}
	printf("\r\n[List]提取数字失败\r\n");
	return NULL;
}
