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



typedef enum smmObjType {
	smmObjType_board = 0,
	smmObjType_card,
	smmObjYype_grade
}smmObjType_e;

typedef enum smmObjGrade {
	smmObjGrade_Ap = 0,
	smmObjGrade_A0,
	smmObjGrade_Am,
	smmObjGrade_Bp,
	smmObjGrade_B0,
	smmObjGrade_Bm,
	smmObjGrade_Cp,
	smmObjGrade_C0,
	smmObjGrade_Cm	
}smmObjGrade_e;

// 1. 구조체 형식 정의
typedef struct smmObject {
	char name[MAX_CHARNAME];
	smmObjType_e ObjType;
	int type;
	int credit;
	int energy;
	smmObjGrade_e grade;
}smmObject_t;



// 3. 관련 함수 변경 

//object generation
void* smmObj_genObject(char* name,smmObjType_e type,int type,int credit,int energy, smmObjGrade_e grade)
{
	smmObj_t* ptr;
	ptr = (smmObject_t *)malloc(sizeof(smmObject_t));
	strcpy(ptr->name,name);
	ptr->ObjType = objType;
	ptr->type = type;
	ptr->credit = credit;
	ptr->energy = energy;
	ptr->grade = grade;

    return ptr;
    
}

// 3. 관련 함수 변경
  
char* smmObj_getNodeName(void* obj)
{
	smmObject_t* ptr = (smmObject_t*)obj; // data encaptualation
	return ptr->name;
}

 
int smmObj_getNodeType(void* obj)
 {
 	smmObject_t*ptr = (smmObject_t*)obj;
	return ptr->type;
}
int smmObj_getNodeCredit(void* obj) 
{
	smmObject_t*ptr = (smmObject_t*)obj;
	return ptr->credit;
}
int smmObj_getNodeEnergy(void* obj)
{
	smmObject_t*ptr = (smmObject_t*)obj;
	return ptr->energy;
}

