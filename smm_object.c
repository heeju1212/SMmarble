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

static char smmObj_name[MAX_NODE][MAX_CHARNAME];
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
static int smmObj_noNode = 0;

static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME];


//object generation
void smmObj_genNode(char* name,int type,int credit,int energy)
{
    strcpy(smmObj_name[smmObj_noNode],name);
	smmObj_type[smmObj_noNode] = type;
    smmObj_credit[smmObj_noNode] = credit;
    smmObj_energy[smmObj_noNode] = energy;
    smmObj_noNode++;
}

// main.c에 함수형태로 name,type,credit,energy를 제공해준다. 
char* smmObj_getNodeName(int node_nr)
{
	return smmObj_name[node_nr];
}
int smmObj_getNodeType(int node_nr)
 {
	return smmObj_type[node_nr];
}
int smmObj_getNodeCredit(int node_nr) 
{
	return smmObj_credit[node_nr];
}
int smmObj_getNodeEnergy(int node_nr)
{
	return smmObj_energy[node_nr];
}






#if 0


//member retrieving



//element to string
char* smmObj_getNodeName(smmNode_e type)
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
#endif
