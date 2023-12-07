//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h


#define SMMNODE_TYPE_LECTURE		0
#define SMMNODE_TYPE_RESTAURANT		1
#define SMMNODE_TYPE_LABORATORY		2
#define SMMNODE_TYPE_HOME			3
#define SMMNODE_TYPE_GOTOLAB		4
#define SMMNODE_TYPE_FOODCHANCE		5
#define SMMNODE_TYPE_FESTIVAL		6

#define SMMNODE_TYPE_MAX			7

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/


/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/

typedef enum smmObjType {
	smmObjType_board = 0,
	smmObjType_card,
	smmObjYype_grade
}smmObjType_e;



//object generation
void smmObj_genNode(char* name,int type,int credit,int energy);

//member retrieving
char* smmObj_getNodeName(void* obj);
int smmObj_getNodeType(void* obj);
int smmObj_getNodeCredit(void* obj);
int smmObj_getNodeEnergy(void* obj);

#if 0
char* smmObj_getNodeName(int node_nr);
int smmObj_getNodeType(int node_nr);
int smmObj_getNodeCredit(int node_nr);
int smmObj_getNodeEnergy(int node_nr);

//element to string

char* smmObj_getTypeName(int type);
#endif


#endif /* smm_object_h */
