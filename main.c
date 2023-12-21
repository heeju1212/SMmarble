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
     // actionNode의 case SMMNODE_TYPE_LECTURE: 부분에 gradePtr을 이용함 
			// smmObjGrade_COUNT 라는요소를 enum smmObjGrade_e 안의 마지막요소로 추가하여, AP,A0,AM,BP,B0,BM,CP,C0,CM라는
			// 성적종류 9개를 random하게 뽑을 수 있도록 하였음.
			// 그렇게 랜덤하게 뽑힌 점수는 gradePtr에 저장됨. 
	 printf("\n(주사위를 던진 플레이어가 들은 과목과 성적)\n");		
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         //printf("%s : %i\n", smmObj_getNodeName(gradePtr), smmObj_getNodeGrade(gradePtr)); 
		 // -> 이 코드는  enum 요소인 성적의 index가 출력되도록 하는 코드이다. 
         
         printf("\n");
        // 사용자에게는  과목명과, 학점과,  알파벳 성적이 출력되어 보이도록 함. 
        // enum의 index 값 대신 알파벳이 출력되도록 하기 위해 55열과 같이 gradenames[] 라는 배열을 정의한 뒤, 
		// enum값이 그 배열의 인덱스값이 되도록 하였다. 
		// 따라서 gradenames[smmObj_getNodeGrade(gradePtr)] 부분을 통해 알파벳 성적이 출력되도록 할 수 있었다.  
         printf("lecture name %s : %d credit, %s grade\n", smmObj_getNodeName(gradePtr),smmObj_getNodeCredit(gradePtr),
		  gradenames[smmObj_getNodeGrade(gradePtr)]);	
     }
}

// 플레이어와 과목명을 매개변수로 하여, 그 과목의 점수의 enum 인덱스를 찾아내는 함수이다. 
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
		printf("Input player %i's name:",i); // 각 플레이어의 이름을 입력하라는 안내 문구
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
    // 사용자가 'g'를 입력하면 지금까지 들은 과목의 이름과 성적을  출력한다. 
        
    return (rand()%MAX_DIE + 1);
    
		
		
	}

int rollDice() {
    return rand() % MAX_DIE + 1; // 주사위를 굴리는 함수 
}
 

// do_experiment 함수 : 실험을 하기 위한 함수이다. 
int do_experiment(int player, int escapeThreshold) {
	int my_dicenum; // my_dicenum: 실험실 노드에서 탈출하기 위해 내가 던진 주사위 숫자에 대한 변수
	my_dicenum = rollDice(); 
	printf("\n");
	// 플레이어가 던진 주사위의 숫자인 my_dicenum이 성공기준값인 escapeThreshold보다 같거나 커야만
	// 플레이어가 실험실을 탈출할 수 있도록 하고, 성공기준값보다 작은 수의 주사위가 나오면 탈출에 실패하도록
	// 하는 코드를 구성하고자 하였다. 
	printf("플레이어가 주사위를 굴려 얻은 값 : %d\n", my_dicenum);
	printf("성공기준값:%d\n",escapeThreshold);
	
	// 플레이어의 탈출여부를 조건에 따라 안내문구로 출력되도록 하였다.
	// flag_escape는 player의 구조체 내부의 멤버변수로 설정하여, 각 플레이어들의 탈출여부 정보를  개별적으로
	// 나타낼 수 있도록 하였다.
	
	// my_dicenum이 escapeThreshold보다 같거나 클 때는 flag_escape의 값을 0으로 반환하여,
	// 실험이 종료되었다는 조건으로 이후에도 활용할 수 있도록 하였다. 
	if (escapeThreshold <= my_dicenum) {
		printf("플레이어가 실험실을 탈출했습니다.\n");
		cur_player[player].flag_escape == 0;
			}
			
	// my_dicenum이 escapeThreshold보다 작을 때는 flag_escape의 값을 1로 반환하여,
	// 아직 실험실을 탈출하지 못했기 때문에, 여전히 '실험 중'이라는  상태를 나타내는 조건으로 이후에도 활용할 수 있도록 하였다. 
	if (escapeThreshold > my_dicenum) {
		
		printf("플레이어가 실험실에서 탈출하는데 실패하였습니다.\n");
		cur_player[player].flag_escape == 1;
		
		}
		
	
	// 함수는 현재 플레이어의 flag_escape(실험 중 여부를 나타내는 상태값)을 반환한다.		
	return cur_player[player].flag_escape;
}


//action code when a player stays at a node
void actionNode(int player)
{
	
	// board에 대한 정보를 가리키는  포인터 변수이다. 
	void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
	// food에 대한 정보를 가리키는  포인터 변수이다. 
	void *foodPtr;
	int i;
	// foodPtr에 foodcard에 대한 정보를 smmdb_getData 함수를 이용하여 전달할 수 있게 하였다. 
	for (i=0;i<smmdb_len(LISTNO_FOODCARD);i++) {
		foodPtr = smmdb_getData(LISTNO_FOODCARD,i);
			}
			
	int type = smmObj_getNodeType(boardPtr);
	char* name = smmObj_getNodeName(boardPtr);
	
	// grade에 대한 정보를 가리키는 포인터변수이다. 
	void *gradePtr;
	
	// foodcard(음식카드)와 festcard(축제 카드)는 랜덤으로 뽑아서 실행되는 카드이므로
	// random하게 구성할 수 있는 코드를 이용하여 아래에 그 내용을 정의하였다. 
	void *foodPtr_rand;
	void *festPtr_rand;
	
	// 실험 중 상태에서 탈출하기 위한 실험 성공값을 나타내는 변수로 "실험실로 이동" 칸에서
	// 주사위를 굴린 결과로 정해지는 값이다.
	// 실험실에 갇힌 플레이어는 이 값보다 같거나 큰 주사위 값을 얻어야만
	// 실험실에서 탈출할 수 있게 된다. 
	int escapeThreshold; 
	
	// player의 turn에 대한 변수
	// 아래와 같은 코드를 통해 플레이어들의 순서가 순환되도록 하였다. 
	int turn;
	turn = (turn+1)%player_nr;
	
		
	// actionNode에서는 
	// 실험 중 상태, flag_escape 변수가 1일 경우에는 강의노드에서 강의를 듣거나, 음식을 먹거나, 축제를 가는 등
	// 다른 노드로 이동하지 못하도록 하고자 하였다.
	// 그래서 강의 노드,식당 노드,foodchance 노드,축제노드,집 등에 flag_escape 값이 0일 때에만 
	// 해당 노드에서 원래 이루어져야하는 동작들이 정상수행되도록 하고자 하는 의도로 각 case의 내용에 if 조건문을 추가하였다.
	
	
	// 그 결과, 어떤 경우에는 정상 실행되어, my_dicenum이 escapeThreshold보다 클 때만 다음 노드로 넘어가고
	// 그렇지 않을 때는 자기 턴이 왔을 때마다  성공할 때까지  실험을 다시 실행하는 결과가 나왔지만,
	// 특정 노드를 지나거나 플레이어의 수가 많아졌을 때 이러한 동작들이 정상적으로 작동하지 않는 경우도 있었다.
	// (실험에 실패했을 때 다시 실험을 해야 하는데 다음 노드로 넘어가버리는 식의 잘못된 작동)
	
	// 이러한 결과에 대해 
	// 조건문의 내용이나 위치가  전체적인 노드의 작동에 완벽하게 들어맞지 않는 부족함이 있거나,
	// 조건문의 내용으로 삼은 flag_escape의 값이 어떤 경우에 의도한대로 설정되지 않아, 
	// 조건문이 잘못된 flag 변수를 받거나 하는 등의 원인을 추측해보았고, 여러 시도를 해 보았으나
	// 여전히 문제조건에 맞게 정상실행되는 경우와 그렇지 않은 경우가 둘 다 발생하는 문제점이 있었다. 
	 
	 
    switch(type)
    {
        //case lecture: 
        case SMMNODE_TYPE_LECTURE:
        	
        	// 실험 중이 아닐 때에만 코드가 작동되도록 하는 조건문이다. 
        	if (cur_player[player].flag_escape == 0) {
        	
        		// 강의를 듣기 위해서 현재에너지가 소요에너지보다 부족할 때에는
				// 에너지가 부족해서 강의를 수강할 수 없다는 안내문구가 출력되도록 하였다. 
            	if (cur_player[player].energy < smmObj_getNodeEnergy(boardPtr)) {
            	 	printf("%s는 에너지가 부족하므로 강의를 수강할 수 없습니다.\n",
				 				cur_player[player].name);
				}
			
				// 이전에 수강한 이력이 있는 강의는 듣지 못하도록 하기 위한 코드이다.
				// 결과적으로 이 부분의 코드를 통해서는 조건이 잘 적용되지는 못하였다.
				// 조건이  실행 결과에 전혀 영향을 끼치지 못하는 모습을 보였다. 
			 
			 
				// 다만, 나의 생각은  findGrade()함수를 통해 이미 수강한 이력이 있는 강의의 점수를 끄집어내와서
				// 그 결과가 빈 문자열(널문자)이면 강의를 아직 한 번도 듣지 않은 것으로 판단하여
				// 정상작동(강의를 듣고 학점을 얻도록 함) 하도록 만들고,
				// 그 결과가 빈 문자열이 아니라면, 강의를 들은 이력이 있는 것으로 판단하여
				// 강의를 다시 듣지 못하도록 막을 수 있다면 문제의 조건을 충족할 수 있지 않을까라는 생각이었다. 
			
				if (findGrade(player, smmObj_getNodeName(boardPtr)) != '\0') {
					 printf("이전에 수강한 이력이 있으므로 수강할 수 없습니다.\n");
				}
			
			
			
			// 현재 에너지가 소요에너지 이상 있고 이전에 듣지 않은 강의이면 수강 가능하다라는 조건을 
			// 구현하기 위한 코드이다. 첫번째 조건은 잘 적용되었고, 
			// 앞서 언급한 바와 같이 두번째 조건은 실행 결과에 전혀 영향을 끼치지 못하는 모습을 보였다. 
			 
			if (cur_player[player].energy >= smmObj_getNodeEnergy(boardPtr) && 
					( findGrade(player, smmObj_getNodeName(boardPtr)) == '\0')) { 
			
			
			// 1. 수강 드랍 선택 
				int choice;
				printf("강의 수강 드랍선택 1-수강 , 2-드랍(숫자입력) :");
				scanf("%d",&choice);
				printf("\n");
				// 1-1. 드랍을 선택시 
				if (choice == 2) {
					printf("\n");
					printf("%s는 강의를 드랍하였습니다.\n",cur_player[player].name);
					printf("\n");
					break;
				}
				// 1-2. 수강을 선택시
				if (choice == 1) {
					printf("\n");
					printf("%s는 강의를 수강하기를 선택하였습니다.\n",cur_player[player].name);
					printf("\n");
        			cur_player[player].accumCredit += smmObj_getNodeCredit(boardPtr);
        			cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr);
        			}
        	
        	
       		
			
        	
			//grade generation
			
			// 성적을 랜덤하게 구현하는 코드
			// smmObjGrade_COUNT 라는요소를 enum smmObjGrade_e 안의 마지막요소로 추가하여, AP,A0,AM,BP,B0,BM,CP,C0,CM라는
			// 성적종류 9개를 random하게 뽑을 수 있도록 하였음.
			// 그렇게 랜덤하게 뽑힌 점수는 gradePtr에 저장됨. 
			
				
            	gradePtr = smmObj_genObject(name, smmObjType_grade, 0, 
					smmObj_getNodeCredit( boardPtr ), 0, rand()%smmObjGrade_COUNT);
            	smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
       		
			}
        	
			break;
			
		}
			
			
		// case home:
		case SMMNODE_TYPE_HOME:
			// 실험 중이 아닐 때에만 코드가 작동되도록 하는 조건문이다.
			if (cur_player[player].flag_escape == 0) {
			
			// 집에 도착하면 지나갈 때마다 지정된 보충에너지만큼의 에너지가 더해지는 코드를 작성하였다. 
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			printf("%s가 집에 도착하여 에너지를 보충합니다.\n");
			printf("\n");
		
			
			break; 
	}
			
			
		// case foodchance:
		case SMMNODE_TYPE_FOODCHANCE:
			// 실험 중이 아닐 때에만 코드가 작동되도록 하는 조건문이다.
			if (cur_player[player].flag_escape == 0) {
			
			// 음식 카드를 LISTNO_FOODCARD 리스트의 내용에서 랜덤으로 뽑도록
			// smmdb_getData함수를 이용하여 작성하였다.
			
			// smmdb_len(LISTNO_FOODCARD)라는 리스트의 길이로 나눈 랜덤값(나머지값)을 
			// smmdb_getData함수에서 요구하는 index값으로 작성하여 음식카드를 랜덤으로 뽑아서
			// foodPtr_rand 변수에 저장되도록 하는 코드를 작성하였다. 
			foodPtr_rand = smmdb_getData(LISTNO_FOODCARD,rand()%smmdb_len(LISTNO_FOODCARD));
			
			// 명시된 보충에너지를 현재 에너지를 더하도록 하는 코드이다. 
			cur_player[player].energy += smmObj_getNodeEnergy(foodPtr_rand);
			
			// 안내 문구 
			printf("%s는 food chance로 %s를 먹었습니다.\n",cur_player[player].name, smmObj_getNodeName(foodPtr_rand));
			printf("\n");
			
			break;
		}
	
		// case restaurant:
		case SMMNODE_TYPE_RESTAURANT:
			// 실험 중이 아닐 때에만 코드가 작동되도록 하는 조건문이다.
			if (cur_player[player].flag_escape == 0)  {
			
			// 보충에너지만큼 플레이어의 현재 에너지에 추가되도록 하는 코드이다. 
			cur_player[player].energy += smmObj_getNodeEnergy(boardPtr);
			
			// 안내 문구 
			printf("%s는 식당에서 에너지를 보충하였습니다.\n",cur_player[player].name);
			printf("\n");
			
			break;
			}
			
			
		// case Gotolab:
		// "실험실로 이동" 칸이다.
		// 실험실로 이동 칸에 도착하면 플레이어의 위치가 실험실로 옮겨지고, 
		// escapeThreshold라는 실험 성공 기준값을 주사위를 굴려 정하게 된다.
		// 또한 flag_escape 변수값이 1(실험 중)으로 바뀌며
		// 위에서 소개된 do_experiment 함수가 실행되도록 하였다. 
		case SMMNODE_TYPE_GOTOLAB:
			escapeThreshold = rollDice();
			cur_player[player].position = 8; 
			cur_player[player].flag_escape == 1;
			cur_player[player].energy -= 3; // 실험을 할 때마다 소모되는 에너지를 반영함 
			do_experiment(turn,escapeThreshold);
			break;
			
		
		// case laboratory:
		// "실험실"(장소) 칸이다. 
		
		case SMMNODE_TYPE_LABORATORY:
			
			// 실험 중 상태(flag_escape값이 1일 때)는 실험이 성공할 때까지 계속 실험을 해야한다.
			// 그래서 조건문의 내부에 do_experiment 함수를 실행하도록 작성하였다.
			  
			if (cur_player[player].flag_escape == 1) {
				
				// 실험을 할 때마다 소모되는 에너지를 반영함
				cur_player[player].energy -= smmObj_getNodeEnergy(boardPtr); 
				do_experiment(turn,escapeThreshold);
			}
			else {	
			// 여기서 else는 
			// "실험실로 이동" 칸을 통해 실험실을 가지 않았고, 그냥 주사위를 던지다가 실험실(장소)에 도착한  경우이다
			// 따라서 이 경우에는  에너지의 변화도 없고, 실험을 하지 않는다. 
			 
				printf("%s는 그냥 실험실을 지나칩니다.\n",cur_player[player].name);
				printf("\n");
			}
			break;
			
		// case festival:
		
		case SMMNODE_TYPE_FESTIVAL:
			// 실험 중이 아닐 때에만 코드가 작동되도록 하는 조건문이다.
			if (cur_player[player].flag_escape == 0)  {
				
			// 축제 카드를 LISTNO_FESTCARD 리스트의 내용에서 랜덤으로 뽑도록
			// smmdb_getData함수를 이용하여 작성하였다.
			
			// smmdb_len(LISTNO_FESTCARD)라는 리스트의 길이로 나눈 랜덤값(나머지값)을 
			// smmdb_getData함수에서 요구하는 index값으로 작성하여 축제카드를 랜덤으로 뽑아서
			// festPtr_rand 변수에 저장되도록 하는 코드를 작성하였다. 
			
			festPtr_rand = smmdb_getData(LISTNO_FESTCARD,rand()%smmdb_len(LISTNO_FESTCARD));
			printf("%s는 '%s' 라는 내용의 미션에 따라 미션을 수행합니다!\n",
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
	
	
	// 마지막 노드의 다음이 첫번째 노드로 연결되는 구조를 구현하기 위한 조건문이다.
	// player의 위치가 노드 바깥(15보다 큰 수)이 되면, 다시 첫번째 노드로 연결되어
	// 이동할 수 있도록 하는 역할을 한다. 
	if (cur_player[player].position > 15) {
		cur_player[player].position -= 15;
		} 
	
	
	boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
	
	// 플레이어가 각 칸으로 이동할 때마다 위치와, 노드의 이름을 출력한다. 
	printf("\n");
	printf("%s go to node %i (name : %s)\n",
				cur_player[player].name, cur_player[player].position,
				smmObj_getNodeName(boardPtr));
	
		}

// 졸업요건을 나타낸 함수 isGraduated()
// flag_graduate 함수의 변화를 추적하여 졸업요건을 충족하지 않은 동안만
// (flag_graduate 값이 0 일 동안만) 
// 보드게임을 진행하도록 하는 코드를 main 함수 내에 구현하였다. 
int isGraduated(int player) {
	// 플레이어 중 한 명이 GRADUATE_CREDIT 이상의 학점을 이수하고
	// 집으로 이동, 또는 집을 지나치면 그 즉시 게임이 종료됨.
	 
	if ((cur_player[player].accumCredit >= GRADUATE_CREDIT) &&
			cur_player[player].position >= 0) {
				// 졸업요건을 만족하면, flag_graduate 변수의 값이 1로 변하고
				// 이것은 졸업요건을 어느 한 플레이어가 만족할 경우 적용된다. 
		cur_player[player].flag_graduate = 1;
	}
	
	// isGraduated 함수는 flag_graduate 값을 반환한다. 
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
    
	// txt파일에서 board,food,festival에 대한 정보를 읽어오는데 이용되는 변수들 
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
        smmdb_addTail(LISTNO_NODE, boardObj); // boardObj라는 구조체 포인터를  리스트에 저장함
		
		// 처음에너지는 집에서의 에너지인 18로 시작한다. 
        if (type == SMMNODE_TYPE_HOME)
        	initEnergy = energy;
        board_nr++;
    }
    
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    // board에 대한 정보를 확인할 수 있는 출력 코드 
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
    
    // foodcard에 대한 정보를 확인할 수 있는 출력 코드 
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
    	smmdb_addTail(LISTNO_FESTCARD, festObj); // festivalObj라는 구조체 포인터를 리스트에 저장함 
		festival_nr++;
        
        
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    // festival card에 대한 정보를 확인할 수 있는 출력 코드 
    for (i=0;i<festival_nr;i++){
    	
    	void* festObj = smmdb_getData(LISTNO_FESTCARD,i);
		
    	printf("festival %i : name %s\n",
			i, smmObj_getNodeName(festObj));
			
	}
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    // 게임에 참여하는 플레이어들을 구성하는 코드
	// 0 이상 MAX_PLAYER 이하 범위의 사람 수로 플레이어를 구성하는 코드 
    do
    {
        //input player number to player_nr
        printf("input player no.:");
    	scanf("%d",&player_nr);
    	fflush(stdin);
        
    }
    while (player_nr < 0 || player_nr > MAX_PLAYER);
    
    // 현재 플레이어에 대한 변수는 동적할당을 이용하여 구성하였다. 
    cur_player = (player_t *)malloc(player_nr*sizeof(player_t));
    // generatePlayers 함수를 통해 각 플레이어의 상태가 정해진다. 
   	generatePlayers(player_nr,initEnergy);
   	
	
    
    
    
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated(turn) == 0) //is anybody graduated?
    {
        int die_result;
        int player;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        
        // 실험 중이 아닐 때에만 코드가 작동되도록 하는 조건문이다.
        if (cur_player[player].flag_escape == 0) {
        	// 각 플레이어가 turn마다 주사위를 던진다. 
        	die_result = rolldie(turn);
    	    }
    	
        //4-3. go forward
        
        printf("\n");
        // 실험 중이 아닐 때에만 코드가 작동되도록 하는 조건문이다.
        if (cur_player[player].flag_escape == 0) {
        	// goForward 함수 내용에 따라 주사위 숫자 결과만큼 플레이어가 이동하도록 한다. 
        	goForward(turn,die_result);
			}
		
		//4-4. take action at the destination node of the board
		
		// actionNode 함수 내용에 따라 각 노드에서 행해져야 하는 동작들이 문제조건에 따라 수행되도록 한다. 
        actionNode(turn);
        
        // isGraduated 함수의 반환값인 flag_graduate 값이 1이 된 경우(즉, 한 명의 플레이어라도 졸업요건을 만족한 경우)에는
		// 게임종료 동작을 실행한다 
		// : 해당 플레이어의 마지막 상태를  출력하고 
		// 이때까지 들은 과목의 이름,학점,성적을 출력한 뒤에는 while문이 곧바로 종료되도록 작성하였다. 
		
        if (isGraduated(turn) == 1) {
        	// 졸업한 플레이어의 마지막 상태인 위치,누적학점,에너지,위치를 반복문 종료 조건이 충족되면  출력하도록 하였다. 
        	printf("%s : credit %i,energy %i,position %i\n",
				cur_player[turn].name,
				cur_player[turn].accumCredit,
				cur_player[turn].energy,
				cur_player[turn].position);
			printf("\n"); 
			// 졸업한 플레이어가 지금까지 들은 과목들의 이름,학점,성적을 모두 출력하는 코드이다.
			printf("졸업한 플레이어가 지금까지 들은 과목들 : \n");
        	printGrades(turn); 
        	// 졸업 요건 만족 후 반복문 종료 
        	break;
		}
        
        //4-5. next turn
        
        // 플레이어들의 순서가 turn마다 순환되도록 하기 위한 코드
		// 졸업요건 만족 후에는 작동하지 않도록 하였다.( 한 명이라도 졸업요건 만족시 그 즉시 종료되도록 만들기 위해서) 
        turn = (turn+1)%player_nr;
    }
    
    
    
    free(cur_player);
    printf("GAME END\n");
    system("PAUSE");
    return 0;
}
