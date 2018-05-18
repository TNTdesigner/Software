#define _CRT_SECURE_NO_WARNINGS
#define ROW 29
#define KOL 100
#define CURSOR(x,y)	printf("\x1b[%d;%dH",y,x)
#define CURSOROF printf("\x1b[?25l")
#define SCORE "score.bin"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
#include <wchar.h>
#include <string.h>

/* Forward declare a type "point" to be a struct. */
typedef struct point point;
/* Declare the struct with integer members x, y */
struct point {
	int    x;											// the kolms
	int    y;											// the rows
};


//functions
void clearfield(char field[][KOL]);
void fruit(char field[][KOL], point snake[], int *lenght, int *exists, int *score, int *scoreOld);
void movesnake(char field[][KOL], point snake[], int *lenght, char *direction, int *fruitExists, int *score, int *gameover, int *pause);
void printfield(char field[][KOL]);
void makeSnake(point snake[], int *length);
void delay(int seconds);
void initTerminal();

//main functie
int main(void) {
	char endGame = ' ';
	initTerminal();										// setup terminal escape codes
	srand(time(NULL));									// setup the randoms whit time
	FILE * fp;											// create a file pointer
	printf("\x1b]2;%sBEL", "ASCII Snake");
	while(endGame != 'q' && endGame != 'Q'){			// run the game till the user presses q or Q
		CURSOR(0, ROW + 1);
		printf("\x1b[60X");								// clear the previos sentence on this line
		CURSOR(KOL + 13, 0);
		printf("\x1b[6X");								// clear the previos sentence on this line
		for (int i = 0; i < 10; i++) {					// clear the score board
			CURSOR(KOL + 2, i + 3);
			printf("\x1b[10X");
		}
		//used variables
	    char field[ROW][KOL];							// the field
		point snake[ROW*KOL];							// the snake
		int scoreBoard[10] = {0,0,0,0,0,0,0,0,0,0};		// the score board to use in game between read and writes
		char names[10][10] = { 
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' },
			{ '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' }, };
		char temp[10] = { '\0' };						// used to clear first element of the names array while shifting happens
		int snakelength = 1;							// the length of the snake
		char dir = '0';									// the direction of the snake
		int fruitExist = 0;								// used as boolean to check if there is stil a fruit in the game
		int gameover = 0;								// boolean to check if you are dead
		int score = 0;									// the score value
		int scoreOld = 0;								// to calculate the spacial frute
		int tempscore = 0;								// temperary score to check on the score board and calculate the delay
		int shift = 0;									// boolean to check if the score boardt has already sifted
		int delayms = 100;								// the starting delay in milli second
		int pause = 0;									// boolean to pause the game

		if ((fp = fopen(SCORE, "rb")) != NULL) {		// if the file exists than read it
			fread(scoreBoard, sizeof(int), 10, fp);		// read the score board
			fread(names, sizeof(char[10]), 10, fp);
			fclose(fp);										// close the file
		}
	

		for (int i = 0; i < 10; i++) {					// print the scoreboard
			CURSOR(KOL + 2, i + 3);
			printf("\x1b[32m\x1b[1m%d)%s\t%d\x1b[0m", i + 1,names[i], scoreBoard[i]);
		}

		clearfield(field);								// clear the field
		makeSnake(snake, &snakelength);					// initialize the snake
		printfield(field);								// clear the console by displaing the empty field

		//main loop of the game
		while (gameover != 1) {
			movesnake(field, snake, &snakelength, &dir, &fruitExist, &score, &gameover, &pause);
			if (fruitExist == 0) {
				fruit(field, snake, &snakelength, &fruitExist, &score, &scoreOld);
			}
			if (scoreOld == (score - 10)) {
				scoreOld = score;
			}
			CURSOR(KOL+2, 0);
			printf("\x1b[1m\x1b[32mYour score: %d", score);
			if (score == (tempscore + 1)) {
				if (delayms != 0) {
					delayms --;
				}
				if (score > scoreBoard[0]) {
					if (shift == 0) {
						for (int i = 9; i>0; i--)
						{
							scoreBoard[i] = scoreBoard[i - 1];
							strcpy(names[i], names[i - 1]);
						}
						shift = 1;
					}
					scoreBoard[0] = score;
					
					strcpy(names[0], temp);
				
					for (int i = 0; i < 10; i++) {
						CURSOR(KOL + 2, i+3);
						printf("\x1b[30X\x1b[1m%d)%s\t%d", i + 1, names[i],scoreBoard[i]);
					}
				}
				tempscore = score;
			}
			delay(delayms);
		}
		CURSOR(KOL/4, ROW/2);
		printf("\x1b[31m ");
		printf("\x1b[1mAt least you tried! Your score: %d", score);	
		if (shift == 1) {
			CURSOR(KOL / 4, (ROW / 2)+1);
			printf("Enter your name [10]: ");
			scanf("%s",&names[0]);
		}
		if ((fp = fopen(SCORE, "wb")) != NULL) {			// if the file exists than read it
			fwrite(scoreBoard, sizeof(int), 10, fp);		// write the score board
			fwrite(names, sizeof(char[10]), 10, fp);
		}
		fclose(fp);											// close the file

		CURSOR(0, ROW + 1);
		printf("\x1b[0mPres any key to continue, pres 'q' to quit.");
		endGame = getch();
	}
}

//initialize the terminal
void initTerminal() {
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}
	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		return GetLastError();
	}
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	dwMode |= DISABLE_NEWLINE_AUTO_RETURN;
	if (!SetConsoleMode(hOut, dwMode))
	{
		return GetLastError();
	}
	CURSOROF;											// make the cursor invisible
}

//funtion for clearing the field
void clearfield(char field[][KOL]) {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < KOL; j++) {
			//print a box arrond the play field
			if (i == 0 && j == 0) {
				field[i][j] = 201;
			}else if (i < ROW -1 && j == 0) {
				field[i][j] = 186;
			}else if (i == 0 && j < KOL - 1) {
				field[i][j] = 205;
			}else if (i == 0 && j == KOL - 1) {
				field[i][j] = 187;
			}else if (i < ROW - 1 && j == KOL -1) {
				field[i][j] = 186;
			}else if (i == ROW -1 && j == KOL - 1) {
				field[i][j] = 188;
			}else if (i == ROW -1 && j == 0){
				field[i][j] = 200;
			}else if (i == ROW - 1 && j < KOL - 1) {
				field[i][j] = 205;
			}else {
				field[i][j] = ' ';
			}
		}
	}
}

//function to make a snake and initialize it
void makeSnake(point snake[], int *lenght) {
	point beginning = { KOL/2,ROW/2 };
	for (int i = *lenght; i >= 0; i--) {
		beginning.x -= i;
		snake[i].x = beginning.x;
		snake[i].y = beginning.y;
	}
}

//function for placing a random fruit
void fruit(char field[][KOL], point snake[], int *lenght, int *exists, int *score, int *scoreOld) {
	int Bsnake = 0; //bool to check if the snake is there
	//create 2 randoms
	int x = rand() % (KOL - 2) + 2;
	int y = rand() % (ROW - 2) + 2;
	for (int i = 0; i < *lenght; i++) {
		if (snake[i].x == x && snake[i].y == y) {
			Bsnake = 1;
			CURSOR(KOL + 2, 1);
		}
	}
	if (Bsnake == 0) {
		*exists = 1;
		field[y][x] = 64;
		CURSOR(x, y);
		printf("\x1b[33m@");
	}
	if (*scoreOld == (*score - 10)) {
		x = rand() % (KOL - 2) + 2;
		y = rand() % (ROW - 2) + 2;
		for (int i = 0; i < *lenght; i++) {
			if (snake[i].x == x && snake[i].y == y) {
				Bsnake = 1;
				CURSOR(KOL + 2, 1);
			}
		}
		if (Bsnake == 0) {
			*exists = 1;
			field[y][x] = '§';
			CURSOR(x, y);
			printf("\x1b[31m§");
		}
	}
}

//move the snake
void movesnake(char field[][KOL],point snake[],int *lenght, char *direction, int *fruitExists,int *score, int *gameover, int *pause) {
	point posHeadNew = snake[0];
	point snakeOld[ROW*KOL];
	char temp;
	int oldLenght = *lenght;
	//copy the snake
	for (int i = 0; i < ROW*KOL; i++) {
		snakeOld[i] = snake[i];
	}
	//check the keyboard input, else keep doing the previos move
	if (_kbhit() != 0) {
		temp = getch();
		//check the arrow key states
		if (GetAsyncKeyState(VK_LEFT)) { temp = '4'; }
		else if (GetAsyncKeyState(VK_UP)) { temp = '8'; }
		else if (GetAsyncKeyState(VK_RIGHT)) { temp = '6'; }
		else if (GetAsyncKeyState(VK_DOWN)) { temp = '2'; }
		else if (GetAsyncKeyState(VK_ESCAPE)) { 
			if (*pause) { *pause = 0;} 
			else { *pause = 1; } } //sleep
	    if (temp == '8' || temp == '6' || temp == '4' || temp == '2') {
			*direction = temp;
		}
	}
	if (*pause == 0) {

		switch (*direction) {
		case 72:
		case '8': //up
				posHeadNew.y--; //y-- because the axis is inverted
				if (posHeadNew.y == 1) {
					*gameover = 1;
				}

			break;
		case 80:
		case '2': //down
				posHeadNew.y++;
				if (posHeadNew.y == ROW){
					*gameover = 1;
				}

			break;
		case 77:
		case '6': //right
				posHeadNew.x++;
				if(posHeadNew.x == KOL) {
					*gameover = 1;
				}
			break;
		case 75:
		case '4': //left
				posHeadNew.x--;
				if (posHeadNew.x == 1) {
					*gameover = 1;
				}
			break;
		default:
				//do nothing
			break;
		}
	
		//no nead of doing this if you are already dead
		if (*gameover == 0) {
			//see if there is a fruite at the new position
			if (field[posHeadNew.y][posHeadNew.x] == 64) {
				*fruitExists = 0;
				*lenght += 1;
				*score += 1;
				field[posHeadNew.y][posHeadNew.x] = ' ';
			}else if (field[posHeadNew.y][posHeadNew.x] == '§') {
				*lenght += 1;
				*score += 1;
				field[posHeadNew.y][posHeadNew.x] = ' ';
			}

			//shift the snake right and add the new head pos
			for (int i = *lenght - 1; i>0; i--)
			{
				snake[i] = snake[i - 1];
			}
			snake[0] = posHeadNew;
		
			//print out the snake and clear the previous space 
			for (int i = 0; i < *lenght; i++) {
				CURSOR(snakeOld[i].x, snakeOld[i].y);
				printf("\x1b[0m ");
				CURSOR(snake[i].x, snake[i].y);
				printf("\x1b[42m");
				printf("\x1b[34m#");
				printf("\x1b[0m");
			}

			//see if the snake is still there or not
			if (*direction != '0') {
				for (int i = 1; i < *lenght; i++) {
					if (snake[i].x == posHeadNew.x && snake[i].y == posHeadNew.y) {
						*gameover = 1;
					}
				}
			}
		}
	}
}

//print the field
void printfield(char field[][KOL]) {
	CURSOR(0, 0);
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < KOL; j++) {
			printf("%c", field[i][j]);
		}
		printf("\n");
	}
}

void delay(int seconds)
{
	int milli_seconds = seconds;
	clock_t start_time = clock();
	while (clock() < start_time + milli_seconds);
}


