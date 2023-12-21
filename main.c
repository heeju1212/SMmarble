//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"



//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

typedef struct player{
	int energy;
	int position;
	int name[MAX_CHARNAME];
	int accumCredit;
	int flag_graduate;
	int flag_escape;
}player_t;

static player_t *cur_player;

// int escapeThreshold; // ���� ��忡�� �ֻ����� ���� �������� ���ϰ� �� ���� ���� ���ذ� 


//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
void generatePlayers(int n, int initEnergy); //generate a new player
void printGrades(int player); //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif


const char* gradenames[] = {"A+", "A0", "A-", "B+","B0","B-","C+","C0","C-"};


void printGrades(int player)
{
     int i;
     void *gradePtr;
     
     // ������ �����ϰ� �����ϴ� �ڵ�
     // actionNode�� case SMMNODE_TYPE_LECTURE: �κп� gradePtr�� �̿��� 
			// smmObjGrade_COUNT ��¿�Ҹ� enum smmObjGrade_e ���� ��������ҷ� �߰��Ͽ�, AP,A0,AM,BP,B0,BM,CP,C0,CM���
			// �������� 9���� random�ϰ� ���� �� �ֵ��� �Ͽ���.
			// �׷��� �����ϰ� ���� ������ gradePtr�� �����. 
	 printf("\n(�ֻ����� ���� �÷��̾ ���� ����� ����)\n");		
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         //printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr)); 
		 // -> �� �ڵ��  enum ����� ������ index�� ��µǵ��� �ϴ� �ڵ��̴�. 
         
         printf("\n");
        // ����ڿ��Դ�  ������, ������,  ���ĺ� ������ ��µǾ� ���̵��� ��. 
        // enum�� index �� ��� ���ĺ��� ��µǵ��� �ϱ� ���� 55���� ���� gradenames[] ��� �迭�� ������ ��, 
		// enum���� �� �迭�� �ε������� �ǵ��� �Ͽ���. 
		// ���� gradenames[smmObj_getNodeGrade(gradePtr)] �κ��� ���� ���ĺ� ������ ��µǵ��� �� �� �־���.  
         printf("lecture name %s : %d credit, %s grade\n", smmObj_getNodeName(gradePtr),smmObj_getNodeCredit(gradePtr),
		  gradenames[smmObj_getNodeGrade(gradePtr)]);	
     }
}

// �÷��̾�� ������� �Ű������� �Ͽ�, �� ������ ������ enum �ε����� ã�Ƴ��� �Լ��̴�. 
int findGrade(int player, char *lectureName) {
	return smmObj_getNodeGrade(lectureName);

}




void printPlayerStatus(void){
	int i;
	for (i=0;i<player_nr;i++) {
		printf("%s : credit %i,energy %i,position %i\n",
		cur_player[i].name,
		cur_player[i].accumCredit,
		cur_player[i].energy,
		cur_player[i].position);
	}
}

void generatePlayers(int n, int initEnergy) { //generate a new player
	int i;
	// n time loop
	for (i=0;i<n;i++){
		// input name
		printf("Input player %i's name:",i); // �� �÷��̾��� �̸��� �Է��϶�� �ȳ� ����
		scanf("%s",cur_player[i].name);
		fflush(stdin); 
		
		// set position
		cur_player[i].position = 0;
		
		// set energy
		cur_player[i].energy= initEnergy;
		cur_player[i].accumCredit = 0;
		cur_player[i].flag_graduate = 0;
		
		
	}
	
	
}


int rolldie(int player)
{	
	
	
    char c;
    printf("\n");
    printf("\n");
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
	

    if (c == 'g') {
    	printGrades(player);
	}
    // ����ڰ� 'g'�� �Է��ϸ� ���ݱ��� ���� ������ �̸��� ������  ����Ѵ�. 
        
    return (rand()%MAX_DIE + 1);
    
		
		
	}

int rollDice() {
    return rand() % MAX_DIE + 1; // �ֻ����� ������ �Լ� 
}
 

// do_experiment �Լ� : ������ �ϱ� ���� �Լ��̴�. 
int do_experiment(int player, int escapeThreshold) {
	int my_dicenum; // my_dicenum: ����� ��忡�� Ż���ϱ� ���� ���� ���� �ֻ��� ���ڿ� ���� ����
	my_dicenum = rollDice(); 
	printf("\n");
	// �÷��̾ ���� �ֻ����� ������ my_dicenum�� �������ذ��� escapeThreshold���� ���ų� Ŀ�߸�
	// �÷��̾ ������� Ż���� �� �ֵ��� �ϰ�, �������ذ����� ���� ���� �ֻ����� ������ Ż�⿡ �����ϵ���
	// �ϴ� �ڵ带 �����ϰ��� �Ͽ���. 
	printf("�÷��̾ �ֻ����� ���� ���� �� : %d\n", my_dicenum);
	printf("�������ذ�:%d\n",escapeThreshold);
	
	// �÷��̾��� Ż�⿩�θ� ���ǿ� ���� �ȳ������� ��µǵ��� �Ͽ���.
	// flag_escape�� player�� ����ü ������ ��������� �����Ͽ�, �� �÷��̾���� Ż�⿩�� ������  ����������
	// ��Ÿ�� �� �ֵ��� �Ͽ���.
	
	// my_dicenum�� escapeThreshold���� ���ų� Ŭ ���� flag_escape�� ���� 0���� ��ȯ�Ͽ�,
	// ������ ����Ǿ��ٴ� �������� ���Ŀ��� Ȱ���� �� �ֵ��� �Ͽ���. 
	if (escapeThreshold <= my_dicenum) {
		printf("�÷��̾ ������� Ż���߽��ϴ�.\n");
		cur_player[player].flag_escape == 0;
			}
			
	// my_dicenum�� escapeThreshold���� ���� ���� flag_escape�� ���� 1�� ��ȯ�Ͽ�,
	// ���� ������� Ż������ ���߱� ������, ������ '���� ��'�̶��  ���¸� ��Ÿ���� �������� ���Ŀ��� Ȱ���� �� �ֵ��� �Ͽ���. 
	if (escapeThreshold > my_dicenum) {
		
		printf("�÷��̾ ����ǿ��� Ż���ϴµ� �����Ͽ����ϴ�.\n");
		cur_player[player].flag_escape == 1;
		
		}
		
	
	// �Լ��� ���� �÷��̾��� flag_escape(���� �� ���θ� ��Ÿ���� ���°�)�� ��ȯ�Ѵ�.		
	return cur_player[player].flag_escape;
}


//action code when a player stays at a node
void actionNode(int player)
{
	
	// board�� ���� ������ ����Ű��  ������ �����̴�. 
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	// food�� ���� ������ ����Ű��  ������ �����̴�. 
	void *foodPtr;
	int i;
	// foodPtr�� foodcard�� ���� ������ smmdb_getData �Լ��� �̿��Ͽ� ������ �� �ְ� �Ͽ���. 
	for (i=0;i<smmdb_len(LISTNO_FOODCARD);i++) {
		foodPtr = smmdb_getData(LISTNO_FOODCARD,i);
			}
			
	int type = smmObj_getNodeType(boardPtr);
	char* name = smmObj_getNodeName(boardPtr);
	
	// grade�� ���� ������ ����Ű�� �����ͺ����̴�. 
	void *gradePtr;
	
	// foodcard(����ī��)�� festcard(���� ī��)�� �������� �̾Ƽ� ����Ǵ� ī���̹Ƿ�
	// random�ϰ� ������ �� �ִ� �ڵ带 �̿��Ͽ� �Ʒ��� �� ������ �����Ͽ���. 
	void *foodPtr_rand;
	void *festPtr_rand;
	
	// ���� �� ���¿��� Ż���ϱ� ���� ���� �������� ��Ÿ���� ������ "����Ƿ� �̵�" ĭ����
	// �ֻ����� ���� ����� �������� ���̴�.
	// ����ǿ� ���� �÷��̾�� �� ������ ���ų� ū �ֻ��� ���� ���߸�
	// ����ǿ��� Ż���� �� �ְ� �ȴ�. 
	int escapeThreshold; 
	
	// player�� turn�� ���� ����
	// �Ʒ��� ���� �ڵ带 ���� �÷��̾���� ������ ��ȯ�ǵ��� �Ͽ���. 
	int turn;
	turn = (turn+1)%player_nr;
	
		
	// actionNode������ 
	// ���� �� ����, flag_escape ������ 1�� ��쿡�� ���ǳ�忡�� ���Ǹ� ��ų�, ������ �԰ų�, ������ ���� ��
	// �ٸ� ���� �̵����� ���ϵ��� �ϰ��� �Ͽ���.
	// �׷��� ���� ���,�Ĵ� ���,foodchance ���,�������,�� � flag_escape ���� 0�� ������ 
	// �ش� ��忡�� ���� �̷�������ϴ� ���۵��� �������ǵ��� �ϰ��� �ϴ� �ǵ��� �� case�� ���뿡 if ���ǹ��� �߰��Ͽ���.
	
	
	// �� ���, � ��쿡�� ���� ����Ǿ�, my_dicenum�� escapeThreshold���� Ŭ ���� ���� ���� �Ѿ��
	// �׷��� ���� ���� �ڱ� ���� ���� ������  ������ ������  ������ �ٽ� �����ϴ� ����� ��������,
	// Ư�� ��带 �����ų� �÷��̾��� ���� �������� �� �̷��� ���۵��� ���������� �۵����� �ʴ� ��쵵 �־���.
	// (���迡 �������� �� �ٽ� ������ �ؾ� �ϴµ� ���� ���� �Ѿ������ ���� �߸��� �۵�)
	
	// �̷��� ����� ���� 
	// ���ǹ��� �����̳� ��ġ��  ��ü���� ����� �۵��� �Ϻ��ϰ� ������ �ʴ� �������� �ְų�,
	// ���ǹ��� �������� ���� flag_escape�� ���� � ��쿡 �ǵ��Ѵ�� �������� �ʾ�, 
	// ���ǹ��� �߸��� flag ������ �ްų� �ϴ� ���� ������ �����غ��Ұ�, ���� �õ��� �� ��������
	// ������ �������ǿ� �°� �������Ǵ� ���� �׷��� ���� ��찡 �� �� �߻��ϴ� �������� �־���. 
	 
	 
    switch(type)
    {
        //case lecture: 
        case SMMNODE_TYPE_LECTURE:
        	
        	// ���� ���� �ƴ� ������ �ڵ尡 �۵��ǵ��� �ϴ� ���ǹ��̴�. 
        	if (cur_player[player].flag_escape == 0) {
        	
        		// ���Ǹ� ��� ���ؼ� ���翡������ �ҿ信�������� ������ ������
				// �������� �����ؼ� ���Ǹ� ������ �� ���ٴ� �ȳ������� ��µǵ��� �Ͽ���. 
            	if (cur_player[player].energy < smmObj_getNodeEnergy(boardPtr)) {
            	 	printf("%s�� �������� �����ϹǷ� ���Ǹ� ������ �� �����ϴ�.\n",
				 				cur_player[player].name);
				}
			
				// ������ ������ �̷��� �ִ� ���Ǵ� ���� ���ϵ��� �ϱ� ���� �ڵ��̴�.
				// ��������� �� �κ��� �ڵ带 ���ؼ��� ������ �� ��������� ���Ͽ���.
				// ������  ���� ����� ���� ������ ��ġ�� ���ϴ� ����� ������. 
			 
			 
				// �ٸ�, ���� ������  findGrade()�Լ��� ���� �̹� ������ �̷��� �ִ� ������ ������ ������ͼ�
				// �� ����� �� ���ڿ�(�ι���)�̸� ���Ǹ� ���� �� ���� ���� ���� ������ �Ǵ��Ͽ�
				// �����۵�(���Ǹ� ��� ������ �򵵷� ��) �ϵ��� �����,
				// �� ����� �� ���ڿ��� �ƴ϶��, ���Ǹ� ���� �̷��� �ִ� ������ �Ǵ��Ͽ�
				// ���Ǹ� �ٽ� ���� ���ϵ��� ���� �� �ִٸ� ������ ������ ������ �� ���� �������� �����̾���. 
			
				if (findGrade(player, smmObj_getNodeName(boardPtr)) != '\0') {
					 printf("������ ������ �̷��� �����Ƿ� ������ �� �����ϴ�.\n");
				}
			
			
			
			// ���� �������� �ҿ信���� �̻� �ְ� ������ ���� ���� �����̸� ���� �����ϴٶ�� ������ 
			// �����ϱ� ���� �ڵ��̴�. ù��° ������ �� ����Ǿ���, 
			// �ռ� ����� �ٿ� ���� �ι�° ������ ���� ����� ���� ������ ��ġ�� ���ϴ� ����� ������. 
			 
			if (cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr) && 
					( findGrade(player, smmObj_getNodeName(boardPtr)) == '\0')) { 
			
			
			// 1. ���� ��� ���� 
				int choice;
				printf("���� ���� ������� 1-���� , 2-���(�����Է�) :");
				scanf("%d",&choice);
				printf("\n");
				// 1-1. ����� ���ý� 
				if (choice == 2) {
					printf("\n");
					printf("%s�� ���Ǹ� ����Ͽ����ϴ�.\n",cur_player[player].name);
					printf("\n");
					break;
				}
				// 1-2. ������ ���ý�
				if (choice == 1) {
					printf("\n");
					printf("%s�� ���Ǹ� �����ϱ⸦ �����Ͽ����ϴ�.\n",cur_player[player].name);
					printf("\n");
        			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			}
        	
        	
       		
			
        	
			//grade generation
			
			// ������ �����ϰ� �����ϴ� �ڵ�
			// smmObjGrade_COUNT ��¿�Ҹ� enum smmObjGrade_e ���� ��������ҷ� �߰��Ͽ�, AP,A0,AM,BP,B0,BM,CP,C0,CM���
			// �������� 9���� random�ϰ� ���� �� �ֵ��� �Ͽ���.
			// �׷��� �����ϰ� ���� ������ gradePtr�� �����. 
			
				
            	gradePtr = smmObj_genObject(name, smmObjType_grade, 0, 
					smmObj_getNodeCredit( boardPtr ), 0, rand()%smmObjGrade_COUNT);
            	smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
       		
			}
        	
			break;
			
		}
			
			
		// case home:
		case SMMNODE_TYPE_HOME:
			// ���� ���� �ƴ� ������ �ڵ尡 �۵��ǵ��� �ϴ� ���ǹ��̴�.
			if (cur_player[player].flag_escape == 0) {
			
			// ���� �����ϸ� ������ ������ ������ ���濡������ŭ�� �������� �������� �ڵ带 �ۼ��Ͽ���. 
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			printf("%s�� ���� �����Ͽ� �������� �����մϴ�.\n");
			printf("\n");
		
			
			break; 
	}
			
			
		// case foodchance:
		case SMMNODE_TYPE_FOODCHANCE:
			// ���� ���� �ƴ� ������ �ڵ尡 �۵��ǵ��� �ϴ� ���ǹ��̴�.
			if (cur_player[player].flag_escape == 0) {
			
			// ���� ī�带 LISTNO_FOODCARD ����Ʈ�� ���뿡�� �������� �̵���
			// smmdb_getData�Լ��� �̿��Ͽ� �ۼ��Ͽ���.
			
			// smmdb_len(LISTNO_FOODCARD)��� ����Ʈ�� ���̷� ���� ������(��������)�� 
			// smmdb_getData�Լ����� �䱸�ϴ� index������ �ۼ��Ͽ� ����ī�带 �������� �̾Ƽ�
			// foodPtr_rand ������ ����ǵ��� �ϴ� �ڵ带 �ۼ��Ͽ���. 
			foodPtr_rand = smmdb_getData(LISTNO_FOODCARD,rand()%smmdb_len(LISTNO_FOODCARD));
			
			// ��õ� ���濡������ ���� �������� ���ϵ��� �ϴ� �ڵ��̴�. 
			cur_player[player].energy += smmObj_getNodeEnergy(foodPtr_rand);
			
			// �ȳ� ���� 
			printf("%s�� food chance�� %s�� �Ծ����ϴ�.\n",cur_player[player].name, smmObj_getNodeName(foodPtr_rand));
			printf("\n");
			
			break;
		}
	
		// case restaurant:
		case SMMNODE_TYPE_RESTAURANT:
			// ���� ���� �ƴ� ������ �ڵ尡 �۵��ǵ��� �ϴ� ���ǹ��̴�.
			if (cur_player[player].flag_escape == 0)  {
			
			// ���濡������ŭ �÷��̾��� ���� �������� �߰��ǵ��� �ϴ� �ڵ��̴�. 
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			
			// �ȳ� ���� 
			printf("%s�� �Ĵ翡�� �������� �����Ͽ����ϴ�.\n",cur_player[player].name);
			printf("\n");
			
			break;
			}
			
			
		// case Gotolab:
		// "����Ƿ� �̵�" ĭ�̴�.
		// ����Ƿ� �̵� ĭ�� �����ϸ� �÷��̾��� ��ġ�� ����Ƿ� �Ű�����, 
		// escapeThreshold��� ���� ���� ���ذ��� �ֻ����� ���� ���ϰ� �ȴ�.
		// ���� flag_escape �������� 1(���� ��)���� �ٲ��
		// ������ �Ұ��� do_experiment �Լ��� ����ǵ��� �Ͽ���. 
		case SMMNODE_TYPE_GOTOLAB:
			escapeThreshold = rollDice();
			cur_player[player].position = 8; 
			cur_player[player].flag_escape == 1;
			cur_player[player].energy -= 3; // ������ �� ������ �Ҹ�Ǵ� �������� �ݿ��� 
			do_experiment(turn,escapeThreshold);
			break;
			
		
		// case laboratory:
		// "�����"(���) ĭ�̴�. 
		
		case SMMNODE_TYPE_LABORATORY:
			
			// ���� �� ����(flag_escape���� 1�� ��)�� ������ ������ ������ ��� ������ �ؾ��Ѵ�.
			// �׷��� ���ǹ��� ���ο� do_experiment �Լ��� �����ϵ��� �ۼ��Ͽ���.
			  
			if (cur_player[player].flag_escape == 1) {
				
				// ������ �� ������ �Ҹ�Ǵ� �������� �ݿ���
				cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr); 
				do_experiment(turn,escapeThreshold);
			}
			else {	
			// ���⼭ else�� 
			// "����Ƿ� �̵�" ĭ�� ���� ������� ���� �ʾҰ�, �׳� �ֻ����� �����ٰ� �����(���)�� ������  ����̴�
			// ���� �� ��쿡��  �������� ��ȭ�� ����, ������ ���� �ʴ´�. 
			 
				printf("%s�� �׳� ������� ����Ĩ�ϴ�.\n",cur_player[player].name);
				printf("\n");
			}
			break;
			
		// case festival:
		
		case SMMNODE_TYPE_FESTIVAL:
			// ���� ���� �ƴ� ������ �ڵ尡 �۵��ǵ��� �ϴ� ���ǹ��̴�.
			if (cur_player[player].flag_escape == 0)  {
				
			// ���� ī�带 LISTNO_FESTCARD ����Ʈ�� ���뿡�� �������� �̵���
			// smmdb_getData�Լ��� �̿��Ͽ� �ۼ��Ͽ���.
			
			// smmdb_len(LISTNO_FESTCARD)��� ����Ʈ�� ���̷� ���� ������(��������)�� 
			// smmdb_getData�Լ����� �䱸�ϴ� index������ �ۼ��Ͽ� ����ī�带 �������� �̾Ƽ�
			// festPtr_rand ������ ����ǵ��� �ϴ� �ڵ带 �ۼ��Ͽ���. 
			
			festPtr_rand = smmdb_getData(LISTNO_FESTCARD,rand()%smmdb_len(LISTNO_FESTCARD));
			printf("%s�� '%s' ��� ������ �̼ǿ� ���� �̼��� �����մϴ�!\n",
				cur_player[player].name, smmObj_getNodeName(festPtr_rand));
			printf("\n");
		
			break;
			
			}
		
			
        default:
            break;
    }
}



void goForward(int player,int step) {
	

	void* boardPtr;
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	int type = smmObj_getNodeType(boardPtr);
	
	
	
	cur_player[player].position+= step;
	
	
	// ������ ����� ������ ù��° ���� ����Ǵ� ������ �����ϱ� ���� ���ǹ��̴�.
	// player�� ��ġ�� ��� �ٱ�(15���� ū ��)�� �Ǹ�, �ٽ� ù��° ���� ����Ǿ�
	// �̵��� �� �ֵ��� �ϴ� ������ �Ѵ�. 
	if (cur_player[player].position > 15) {
		cur_player[player].position -= 15;
		} 
	
	
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
	
	// �÷��̾ �� ĭ���� �̵��� ������ ��ġ��, ����� �̸��� ����Ѵ�. 
	printf("\n");
	printf("%s go to node %i (name : %s)\n",
				cur_player[player].name, cur_player[player].position,
				smmObj_getNodeName(boardPtr));
	
		}

// ��������� ��Ÿ�� �Լ� isGraduated()
// flag_graduate �Լ��� ��ȭ�� �����Ͽ� ��������� �������� ���� ���ȸ�
// (flag_graduate ���� 0 �� ���ȸ�) 
// ��������� �����ϵ��� �ϴ� �ڵ带 main �Լ� ���� �����Ͽ���. 
int isGraduated(int player) {
	// �÷��̾� �� �� ���� GRADUATE_CREDIT �̻��� ������ �̼��ϰ�
	// ������ �̵�, �Ǵ� ���� ����ġ�� �� ��� ������ �����.
	 
	if ((cur_player[player].accumCredit >= GRADUATE_CREDIT) &&
			cur_player[player].position >= 0) {
				// ��������� �����ϸ�, flag_graduate ������ ���� 1�� ���ϰ�
				// �̰��� ��������� ��� �� �÷��̾ ������ ��� ����ȴ�. 
		cur_player[player].flag_graduate = 1;
	}
	
	// isGraduated �Լ��� flag_graduate ���� ��ȯ�Ѵ�. 
	return cur_player[player].flag_graduate;
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn = 0;
    
	// txt���Ͽ��� board,food,festival�� ���� ������ �о���µ� �̿�Ǵ� ������ 
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    void *gradePtr;
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp,"%s %i %i %i",name,&type,&credit,&energy) == 4) //read a node parameter set
    {
        //store the parameter set
        void* boardObj = smmObj_genObject(name,smmObjType_board, type,credit,energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj); // boardObj��� ����ü �����͸�  ����Ʈ�� ������
		
		// ó���������� �������� �������� 18�� �����Ѵ�. 
        if (type == SMMNODE_TYPE_HOME)
        	initEnergy = energy;
        board_nr++;
    }
    
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    // board�� ���� ������ Ȯ���� �� �ִ� ��� �ڵ� 
    for (i=0;i<board_nr;i++){
    	void* boardObj = smmdb_getData(LISTNO_NODE,i);
		
    	printf("node %i : %s, %i(%s), credit %i, energy %i\n",
			i, smmObj_getNodeName(boardObj),smmObj_getNodeType(boardObj), smmObj_getTypeName(smmObj_getNodeType(boardObj)),
			smmObj_getNodeCredit(boardObj),smmObj_getNodeEnergy(boardObj));
			
	}

    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while (fscanf(fp,"%s %i",name,&energy) == 2) //read a food parameter set
    {	//store the parameter set
    	void* foodObj = smmObj_genObject(name,smmObjType_foodcard,SMMNODE_TYPE_FOODCHANCE,0,energy,0);
    	smmdb_addTail(LISTNO_FOODCARD, foodObj); // foodObj��� ����ü �����͸� ����Ʈ�� ������ 
		food_nr++;
    }
    
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    // foodcard�� ���� ������ Ȯ���� �� �ִ� ��� �ڵ� 
    for (i=0;i<food_nr;i++){
    	
    	void* foodObj = smmdb_getData(LISTNO_FOODCARD,i);
		
    	printf("food %i : name %s , energy %i\n",
			i, smmObj_getNodeName(foodObj),smmObj_getNodeEnergy(foodObj));
			
	}
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp,"%s",name) == 1) //read a festival card string
    {
        //store the parameter set
        void* festObj = smmObj_genObject(name,smmObjType_festcard,SMMNODE_TYPE_FESTIVAL,0,0,0);
    	smmdb_addTail(LISTNO_FESTCARD, festObj); // festivalObj��� ����ü �����͸� ����Ʈ�� ������ 
		festival_nr++;
        
        
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    // festival card�� ���� ������ Ȯ���� �� �ִ� ��� �ڵ� 
    for (i=0;i<festival_nr;i++){
    	
    	void* festObj = smmdb_getData(LISTNO_FESTCARD,i);
		
    	printf("festival %i : name %s\n",
			i, smmObj_getNodeName(festObj));
			
	}
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    // ���ӿ� �����ϴ� �÷��̾���� �����ϴ� �ڵ�
	// 0 �̻� MAX_PLAYER ���� ������ ��� ���� �÷��̾ �����ϴ� �ڵ� 
    do
    {
        //input player number to player_nr
        printf("input player no.:");
    	scanf("%d",&player_nr);
    	fflush(stdin);
        
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);
    
    // ���� �÷��̾ ���� ������ �����Ҵ��� �̿��Ͽ� �����Ͽ���. 
    cur_player = (player_t *)malloc(player_nr*sizeof(player_t));
    // generatePlayers �Լ��� ���� �� �÷��̾��� ���°� ��������. 
   	generatePlayers(player_nr,initEnergy);
   	
	
    
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated(turn) == 0) //is anybody graduated?
    {
        int die_result;
        int player;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        
        // ���� ���� �ƴ� ������ �ڵ尡 �۵��ǵ��� �ϴ� ���ǹ��̴�.
        if (cur_player[player].flag_escape == 0) {
        	// �� �÷��̾ turn���� �ֻ����� ������. 
        	die_result = rolldie(turn);
    	    }
    	
        //4-3. go forward
        
        printf("\n");
        // ���� ���� �ƴ� ������ �ڵ尡 �۵��ǵ��� �ϴ� ���ǹ��̴�.
        if (cur_player[player].flag_escape == 0) {
        	// goForward �Լ� ���뿡 ���� �ֻ��� ���� �����ŭ �÷��̾ �̵��ϵ��� �Ѵ�. 
        	goForward(turn,die_result);
			}
		
		//4-4. take action at the destination node of the board
		
		// actionNode �Լ� ���뿡 ���� �� ��忡�� �������� �ϴ� ���۵��� �������ǿ� ���� ����ǵ��� �Ѵ�. 
        actionNode(turn);
        
        // isGraduated �Լ��� ��ȯ���� flag_graduate ���� 1�� �� ���(��, �� ���� �÷��̾�� ��������� ������ ���)����
		// �������� ������ �����Ѵ� 
		// : �ش� �÷��̾��� ������ ���¸�  ����ϰ� 
		// �̶����� ���� ������ �̸�,����,������ ����� �ڿ��� while���� ��ٷ� ����ǵ��� �ۼ��Ͽ���. 
		
        if (isGraduated(turn) == 1) {
        	// ������ �÷��̾��� ������ ������ ��ġ,��������,������,��ġ�� �ݺ��� ���� ������ �����Ǹ�  ����ϵ��� �Ͽ���. 
        	printf("%s : credit %i,energy %i,position %i\n",
				cur_player[turn].name,
				cur_player[turn].accumCredit,
				cur_player[turn].energy,
				cur_player[turn].position);
			printf("\n"); 
			// ������ �÷��̾ ���ݱ��� ���� ������� �̸�,����,������ ��� ����ϴ� �ڵ��̴�.
			printf("������ �÷��̾ ���ݱ��� ���� ����� : \n");
        	printGrades(turn); 
        	// ���� ��� ���� �� �ݺ��� ���� 
        	break;
		}
        
        //4-5. next turn
        
        // �÷��̾���� ������ turn���� ��ȯ�ǵ��� �ϱ� ���� �ڵ�
		// ������� ���� �Ŀ��� �۵����� �ʵ��� �Ͽ���.( �� ���̶� ������� ������ �� ��� ����ǵ��� ����� ���ؼ�) 
        turn = (turn+1)%player_nr;
    }
    
    
    
    free(cur_player);
    printf("GAME END\n");
    system("PAUSE");
    return 0;
}
