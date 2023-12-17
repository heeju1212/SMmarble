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

// int escapeThreshold; // 실험 노드에서 주사위를 굴려 랜덤으로 정하게 될 실험 성공 기준값 


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
     
     // 성적을 랜덤하게 구현하는 코드
     // actionNode의 case SMMNODE_TYPE_LECTURE: 부분에 이어서 작성하였음. 
			// smmObjGrade_COUNT 라는요소를 enum smmObjGrade_e 안의 마지막요소로 추가하여, AP,A0,AM,BP,B0,BM,CP,C0,CM라는
			// 성적종류 9개를 random하게 뽑을 수 있도록 하였음.
			// 그렇게 랜덤하게 뽑힌 점수는 gradePtr에 저장됨. 
			
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         //printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr)); // enum 요소인 성적의 index가 출력됨 
         
         printf("%s : %s\n", smmObj_getNodeName(gradePtr), gradenames[smmObj_getNodeGrade(gradePtr)]);	 // 사용자에게 알파벳 성적이 출력되어 보이도록 함. 
     }
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
		printf("Input player %i's name:",i); // 안내 문구
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
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    

    if (c == 'g') {
    	printGrades(player);
	}
    // 지금까지 들은 과목 출력 
        
    return (rand()%MAX_DIE + 1);
}

int rollDice() {
    return rand() % MAX_DIE + 1; // 주사위를 굴리는 함수 
}

int do_experiment(int player, int escapeThreshold) {
	int my_dicenum; // 실험실 노드에서 탈출하기 위해 내가 던진 주사위 숫자에 대한 변수
	// cur_player[player].flag_escape = 1;
	int cnt = 0;
	while (cnt == 0){
		my_dicenum = rollDice(); 
		printf("\n");
		printf("플레이어가 주사위를 굴려 얻은 값 : %d\n", my_dicenum);
		printf("성공기준값:%d\n",escapeThreshold);
		cnt++;
		}
		if (escapeThreshold <= my_dicenum) {
			printf("%s가 실험실을 탈출했습니다.\n",cur_player[player].name);
			cur_player[player].flag_escape == 0;
			}
		if (escapeThreshold > my_dicenum) {
		
			printf("%s가 실험실에서 탈출하는데 실패하였습니다.\n",cur_player[player].name);
			cur_player[player].flag_escape == 1;
		
		}

			
	return cur_player[player].flag_escape;
}


//action code when a player stays at a node
void actionNode(int player)
{
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	void *foodPtr;
	int i;
	for (i=0;i<smmdb_len(LISTNO_FOODCARD);i++) {
		foodPtr = smmdb_getData(LISTNO_FOODCARD,i);
			}
	int type = smmObj_getNodeType(boardPtr);
	char* name = smmObj_getNodeName(boardPtr);
	void *gradePtr;
	void *foodPtr_rand;
	int escapeThreshold; 
	int turn;
	turn = (turn+1)%player_nr;
	
	
	
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
            if (cur_player[player].energy < smmObj_getNodeEnergy(boardPtr)) {
            	 printf("%s는 에너지가 부족하므로 강의를 수강할 수 없습니다.\n",
				 			cur_player[player].name);
            	 break;
			}
			
			#if 0 
			if ( "이전에 수강한 이력이 있는 강의라면") {
				 printf("이전에 수강한 이력이 있으므로 수강할 수 없습니다.");
			}
			#endif
			
        	cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        	cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        	
			//grade generation
			
			// 성적을 랜덤하게 구현하는 코드
			// smmObjGrade_COUNT 라는요소를 enum smmObjGrade_e 안의 마지막요소로 추가하여, AP,A0,AM,BP,B0,BM,CP,C0,CM라는
			// 성적종류 9개를 random하게 뽑을 수 있도록 하였음.
			// 그렇게 랜덤하게 뽑힌 점수는 gradePtr에 저장됨. 
			
            gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getNodeCredit( boardPtr ), 0, rand()%smmObjGrade_COUNT);
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
        	
        	
			break;
		// case home:
		case SMMNODE_TYPE_HOME:
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			printf("%s가 집에 도착하여 에너지를 보충합니다.\n");
			printf("\n");
			
		// case foodchance:
		case SMMNODE_TYPE_FOODCHANCE:
			foodPtr_rand = smmdb_getData(LISTNO_FOODCARD,rand()%smmdb_len(LISTNO_FOODCARD));
			cur_player[player].energy += smmObj_getNodeEnergy(foodPtr_rand);
			printf("%s는 food chance로 %s를 먹었습니다.\n",cur_player[player].name, smmObj_getNodeName(foodPtr_rand));
			printf("\n");
			break;
		// case restaurant:
		case SMMNODE_TYPE_RESTAURANT:
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			printf("%s는 식당에서 에너지를 보충하였습니다.\n",cur_player[player].name);
			printf("\n");
			break;
		// case Gotolab:
		case SMMNODE_TYPE_GOTOLAB:
			escapeThreshold = rollDice();
			cur_player[player].position = 8; 
			cur_player[player].flag_escape == 1;
			do_experiment(turn,escapeThreshold);
			break;
			
		
		// case laboratory:
		
		case SMMNODE_TYPE_LABORATORY:
			if (cur_player[player].flag_escape == 1) {
				do_experiment(turn,escapeThreshold);
				cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
				break;
			}
			else {	
				printf("%s는 그냥 실험실을 지나칩니다.\n",cur_player[player].name);
			}
	
			
        default:
            break;
    }
}



void goForward(int player,int step) {
	
	
	void* boardPtr;
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	int type = smmObj_getNodeType(boardPtr);
	
	int cnt_exp = 0;
	int turn = 0;
	
	cur_player[player].position+= step;
	
	if (cur_player[player].position > 15) {
		cur_player[player].position -= 15;
		} 
	
	
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
	
	
		
	printf("%s go to node %i (name : %s)\n",
				cur_player[player].name, cur_player[player].position,
				smmObj_getNodeName(boardPtr));
	
    
}

    
int isGraduated(int player) {
	if ((cur_player[player].accumCredit >= GRADUATE_CREDIT) &&
			cur_player[player].position == SMMNODE_TYPE_HOME) {
		cur_player[player].flag_graduate = 1;
	}
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
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
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
        smmdb_addTail(LISTNO_NODE, boardObj); // boardObj라는 구조체 포인터를  리스트에 저장함 
        if (type == SMMNODE_TYPE_HOME)
        	initEnergy = energy;
        board_nr++;
    }
    
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
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
    	smmdb_addTail(LISTNO_FOODCARD, foodObj); // foodObj라는 구조체 포인터를 리스트에 저장함 
		food_nr++;
    }
    
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    for (i=0;i<food_nr;i++){
    	
    	void* foodObj = smmdb_getData(LISTNO_FOODCARD,i);
		
    	printf("food %i : name %s , energy %i\n",
			i, smmObj_getNodeName(foodObj),smmObj_getNodeEnergy(foodObj));
			
	}
    
    
    #if 0
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    #endif
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("input player no.:");
    	scanf("%d",&player_nr);
    	fflush(stdin);
        
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);
    cur_player = (player_t *)malloc(player_nr*sizeof(player_t));
   	generatePlayers(player_nr,initEnergy);
   	
	
    
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated(turn) == 0) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //4-3. go forward
        printf("\n");
        goForward(turn,die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn+1)%player_nr;
    }
    printf("졸업한 플레이어가 수강한 강의의 이름,학점,성적을 각각 출력\n");
    
    free(cur_player);
    printf("GAME END\n");
    system("PAUSE");
    return 0;
}
