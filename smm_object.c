//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE 		100


static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME] = {
	"lecture",
	"restaurant",
	"laboratory",
	"home",
	"gotoLab",
	"foodChance",
	"festival"
};

char* smmObj_getTypeName(int type){
	return (char*)smmNodeName[type];
}

// 1. 구조체 형식 정의

typedef struct smmObject {
	char name[MAX_CHARNAME];
	int type;
	int credit;
	int energy;
}smmObject_t;

// 2. 구조체 배열 변수 정의 

static smmObject_t smm_node[MAX_NODE];


static int smmObj_noNode = 0;


// 3. 관련 함수 변경 

//object generation
void smmObj_genNode(char* name,int type,int credit,int energy)
{
	strcpy(smm_node[smmObj_noNode].name,name);
	smm_node[smmObj_noNode].type = type;
    smm_node[smmObj_noNode].credit = credit;
    smm_node[smmObj_noNode].energy = energy;
    
    smmObj_noNode++;

}

// 3. 관련 함수 변경(구조체 형태) 
 
// main.c에 함수형태로 name,type,credit,energy를 제공해준다. 
char* smmObj_getNodeName(int node_nr)
{
	return smm_node[node_nr].name;
}
int smmObj_getNodeType(int node_nr)
 {
	return smm_node[node_nr].type;
}
int smmObj_getNodeCredit(int node_nr) 
{
	return smm_node[node_nr].credit;
}
int smmObj_getNodeEnergy(int node_nr)
{
	return smm_node[node_nr].energy;
}

