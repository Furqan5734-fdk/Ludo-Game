//============================================================================
// Name        : .cpp
// Author      : Furqan Haider
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Centipede...
//============================================================================

#ifndef CENTIPEDE_CPP_
#define CENTIPEDE_CPP_
//#include "Board.h"
#include "util.h"
#include <iostream>
#include<string>
#include<cmath> // for basic math functions such as cos, sin, sqrt
using namespace std;

int firstRun = 1;
struct Location {
	int x; int y;
};

struct Location locMap[72];
struct Location homeSquareOffset[4];
int pawnPosition[16];
int currentPlayer;
int pawnsInJail[4];
int pawnsInHome[4];
int validMoves[4];
int selectedMove = -1;
int winState = 0;
const char *playerNames[4] = { "Red", "Yellow", "Green", "Blue" };
float *playerColors[4] = {colors[SALMON], colors[YELLOW], colors[LIGHT_GREEN], colors[LIGHT_BLUE]};
int hltdSquare;
int currentRoll[3];
int resetGame = 1;
int movesLeft = 0;
int posTable_0[] = {0,3,6,9,12,15,33,30,27,24,21,18,19,20,23,26,29,32,35,36,39,42,45,48,51,52,53,50,47,44,41,38,56,59,62,65,68,71,70,69,66,63,60,57,54,17,14,11,8,5,2,1,4, 7, 10, 13, 16};
int posTable_1[] = {0,23,26,29,32,35,36,39,42,45,48,51,52,53,50,47,44,41,38,56,59,62,65,68,71,70,69,66,63,60,57,54,17,14,11,8,5,2,1,0,3,6,9,12,15,33,30,27,24,21,18,19, 22, 25, 28, 31, 34};
int posTable_2[] = {0,50,47,44,41,38,56,59,62,65,68,71,70,69,66,63,60,57,54,17,14,11,8,5,2,1,0,3,6,9,12,15,33,30,27,24,21,18,19,20,23,26,29,32,35,36,39,42,45,48,51,52,49, 46, 43, 40, 37};
int posTable_3[] = {0,66,63,60,57,54,17,14,11,8,5,2,1,0,3,6,9,12,15,33,30,27,24,21,18,19,20,23,26,29,32,35,36,39,42,45,48,51,52,53,50,47,44,41,38,56,59,62,65,68,71,70, 67, 64, 61, 58, 55};
int safeSquares[] = { 3, 8, 24, 23, 45, 50, 65, 66 };
const int PAWN_IN_JAIL = -1;
const int PAWN_HOME = -2;


// seed the random numbers generator by current time (see the documentation of srand for further help)...

/* Function sets canvas size (drawing area) in pixels...
 *  that is what dimensions (x and y) your game will have
 *  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
 * */
void SetCanvasSize(int width, int height) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}


void DrawPawn(int x, int y, int player) {
	int trianglex1 = x + player*3 + 5;
	int triangley1 = y + player*5 + 5;
	int trianglex2 = trianglex1 + 30;
	int trianglex3 = trianglex1 + 10;
	int triangley3 = triangley1 + 40;
	DrawTriangle( trianglex1, triangley1 , trianglex2, triangley1 , trianglex3 , triangley3, playerColors[player]);
}

int RollDice(){
	currentRoll[0] = rand() % (6 + 1 - 1) + 1;
	if(currentRoll[0] == 6) {
		currentRoll[1] = rand() % (6 + 1 - 1) + 1;
		if(currentRoll[1] == 6) {
			currentRoll[2] = rand() % (6 + 1 - 1) + 1;
			if (currentRoll[2] = 6) {
				return 0;
			}
		} else {
			currentRoll[2] = 0;
		}
	} else {
		currentRoll[1] = 0; currentRoll[2] = 0;
		return 0;
	}
	return 1;
}

void CyclePlayer(){
	if(currentPlayer < 3){
		currentPlayer++;
	} else {
		currentPlayer = 0;
	}
}


int CheckValidMoves(int roll){
	std::fill_n(validMoves, 4, 0);
	//incase of all sixes
	if(currentRoll[0] == 6 && currentRoll[1] == 6 && currentRoll[2] == 6){return 0;}
	//selectedMove = -1;
	int offset = currentPlayer*4;
	
	// only valid move is moving out of jail
	if(pawnsInJail[currentPlayer] + pawnsInHome[currentPlayer] == 4 && roll == 6){
		validMoves[0] = -1;
		validMoves[1] = 0;
		validMoves[2] = 0;
		validMoves[3] = 0;
		return 1;
	} else if (pawnsInJail[currentPlayer] + pawnsInHome[currentPlayer] == 4 && roll != 6) {
		// NO Valid moves
		return 0;
	}

	// for all other moves
	for(int i=0; i < 4; ++i){
		if (pawnPosition[offset+i] == PAWN_IN_JAIL && roll == 6) { //if pawn is in jail
			validMoves[i] = -1;
		} else if (pawnPosition[offset+i] == PAWN_HOME) { //if pawn is home
			validMoves[i] = 0;
		} else if (pawnPosition[offset+i] + roll <= 57 && pawnPosition[offset+i] != -1) { //check if roll is longer than distance to home
			validMoves[i] = pawnPosition[offset+i];
		} else if (pawnPosition[offset+i] + roll > 57) {
			validMoves[i] = 0;
		}
	}
	return 1;
}

void ResetGame(){
	homeSquareOffset[0].x = 162; homeSquareOffset[0].y = 162;
	homeSquareOffset[1].x = 162; homeSquareOffset[1].y = 748;
	homeSquareOffset[2].x = 748; homeSquareOffset[2].y = 748;
	homeSquareOffset[3].x = 748; homeSquareOffset[3].y = 162;

	std::fill_n(pawnPosition, 16, -1);
	std::fill_n(pawnsInJail, 4, 4);
	std::fill_n(pawnsInHome, 4, 0);
	currentPlayer = 0; // TODO: change to random
	hltdSquare = -2; //TODO: change to -2 for no square; position -1 will be jail square of current player
	while(RollDice() == 0){
		CyclePlayer();
	}
	CheckValidMoves(currentRoll[0]);
}

void CheckPawnOverlap(int pawn){
	int pos1;
	if (currentPlayer == 0) { pos1 = posTable_0[pawnPosition[pawn]];}
	else if (currentPlayer == 1) {pos1 = posTable_1[pawnPosition[pawn]];}
	else if (currentPlayer == 2) {pos1 = posTable_2[pawnPosition[pawn]];}
	else if (currentPlayer == 3) {pos1 = posTable_3[pawnPosition[pawn]];}

	// check if safe square
	for(int i=0; i < 8; ++i){
		if(pos1 == safeSquares[i]) { return; }
	}
	
	for(int i=0; i < 16; ++i){
		// if pawn in jail
		if(pawnPosition[i] == PAWN_IN_JAIL) {continue;}

		int player = 0;
		int pos2;
		if(i > 3) { player = 1; }
		if(i > 7) { player = 2; }
		if(i > 11) { player = 3; }

		// same player can overlap
		if(player == currentPlayer) {continue;}

		if(player == 0){ pos2 = posTable_0[pawnPosition[i]];}
		else if(player == 1){ pos2 = posTable_1[pawnPosition[i]]; }
		else if(player == 2){ pos2 = posTable_2[pawnPosition[i]]; }
		else if(player == 3){ pos2 = posTable_3[pawnPosition[i]]; }
		
		//if overlap
		if(pos1 == pos2) {
			//send to jail
			cout << "Position overlaps, 1: " << pos1 << " ,2: " << pos2  <<endl;
			pawnPosition[i] = PAWN_IN_JAIL;
			pawnsInJail[player] += 1;
		}
	}
}

void MovePawn(){
	int offset = currentPlayer*4;
	for(int i=0; i<4; ++i){
		if(pawnPosition[offset + i] == hltdSquare){
			if(pawnPosition[offset + i] == PAWN_IN_JAIL){
				pawnPosition[offset + i] = 1;
				pawnsInJail[currentPlayer] -= 1;
			} else {
				// is home?
				if(pawnPosition[offset + i] + currentRoll[0] == 57){
					cout << "pawn in home" << endl;
					pawnPosition[offset + i] = PAWN_HOME;
					pawnsInHome[currentPlayer] +=1;
				} else {
					// move pawn to new position
					pawnPosition[offset + i] += currentRoll[0];
					// check if another pawn is on same position
					CheckPawnOverlap(offset + i);
				}
			}
			// Shift current roll
			currentRoll[0] = currentRoll[1];
			currentRoll[1] = currentRoll[2];
			currentRoll[2] = 0;
			// check if currentRoll[0] zero continue to next player; and reroll Dice
			if (currentRoll[0] == 0){
				CyclePlayer();
				RollDice();
			}
			// remove highlight
			hltdSquare = -2;
			return;
		}
	}
}

/*
 * Main Canvas drawing function.
 * */
//Board *b;
void GameDisplay()/**/{
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim rid and 1 means pure red and so on.
	
	//reset game
	if(resetGame == 1){
		ResetGame();
		resetGame = 0;
	}

	glClearColor(0/*Red Component*/, 0,	//148.0/255/*Green Component*/,
			0/*Blue Component*/, 255 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear (GL_COLOR_BUFFER_BIT); //Update the colors

	// if all pawns are in home
	for(int i; i < 4; ++i){
		if(pawnsInHome[i] == 4){
			winState = 1;
			currentPlayer = i;
			//glutPostRedisplay();
			//return 0;
		}
	}

	if(winState == 1){
		char str[20];
		sprintf(str, "%s has won the game!", playerNames[currentPlayer]);
		DrawString(420, 450, str, colors[WHITE]);
		sprintf(str, "Press r to reset", playerNames[currentPlayer]);
		DrawString(420, 400, str, colors[WHITE]);
		glutSwapBuffers();
		return;
	}

	int homeSquareSize = 390;
	int gridSize = 65;

	//home squares
	DrawSquare(0, 0, homeSquareSize, colors[RED]);
	DrawSquare(homeSquareSize + 195, 0, homeSquareSize, colors[BLUE]);
	DrawSquare(homeSquareSize + 195, homeSquareSize + 195, homeSquareSize, colors[GREEN]);
	DrawSquare(0, homeSquareSize + 195, homeSquareSize, colors[OLIVE]);

	int offsetx = 390;
	int offsety = 0;

	for (int i = 0; i < 6; ++i) {
		for (int n = 0; n < 3; ++n) {
			int position = (i*3) + n;
			float* color = colors[WHITE];
			if( position == 3 || position == 4 || position == 7 || position == 10 || position == 13 || position == 16 ){
				color = colors[RED];
			} else if ( position == 8 ){
				color = colors[PINK];
			}
			DrawSquare( offsetx + (n*gridSize), offsety + (i*gridSize), gridSize, color);
			if(firstRun == 1){
				locMap[position].x = offsetx + (n*gridSize);
				locMap[position].y = offsety + (i*gridSize);
			}
		}
	}
	offsetx = 390;
	offsety = 975 - 390;
	for (int i = 0; i < 6; ++i) {
		for (int n = 0; n < 3; ++n) {
			int position = (i*3) + n + 36;
			float* color = colors[WHITE];
			if( position == 50 || position == 49 || position == 46 || position == 43 || position == 40 || position == 37 ){
				color = colors[GREEN];
			} else if ( position == 45 ){
				color = colors[PINK];
			}
			DrawSquare( offsetx + (n*gridSize), offsety + (i*gridSize), gridSize, color);
			if(firstRun == 1){
				locMap[position].x = offsetx + (n*gridSize);
				locMap[position].y = offsety + (i*gridSize);
			}
		}
	}

	offsetx = 0;
	offsety = 390;
	for (int i = 0; i < 3; ++i) {
		for (int n = 0; n < 6; ++n) {
			int position = (n*3) + i + 18;
			float* color = colors[WHITE];
			if( position == 23 || position == 28 || position == 25 || position == 22 || position == 31 || position == 34){
				color = colors[OLIVE];
			} else if ( position == 24 ){
				color = colors[PINK];
			}
			DrawSquare( offsetx + (n*gridSize), offsety + (i*gridSize), gridSize, color);
			if(firstRun == 1){
				locMap[position].x = offsetx + (n*gridSize);
				locMap[position].y = offsety + (i*gridSize);
			}
		}
	}

	offsetx = 975 - 390;
	offsety = 390;
	for (int i = 0; i < 3; ++i) {
		for (int n = 0; n < 6; ++n) {
			int position = (n*3) + i + 54;
			float* color = colors[WHITE];
			if( position == 66 || position == 67 || position == 64 || position == 61 || position == 58 || position == 55 ){
				color = colors[BLUE];
			} else if ( position == 65 ){
				color = colors[PINK];
			}
			DrawSquare( offsetx + (n*gridSize), offsety + (i*gridSize), gridSize, color);
			if(firstRun == 1){
				locMap[position].x = offsetx + (n*gridSize);
				locMap[position].y = offsety + (i*gridSize);
			}
		}
	}

	// dice square
	DrawSquare(390, 390, 195, colors[BEIGE]);

	// draw current roll
	char str[20];
	sprintf(str, "%d %d %d", currentRoll[0], currentRoll[1], currentRoll[2]);
	DrawString(470, 420, str, colors[BLACK]);

	// draw current player name
	sprintf(str, "Turn: %s", playerNames[currentPlayer]);
	DrawString(420, 450, str, colors[BLACK]);

	// draw score
	sprintf(str, "R: %d Y: %d G: %d B: %d", pawnsInHome[0], pawnsInHome[1], pawnsInHome[2], pawnsInHome[3]);
	DrawString(395, 500, str, colors[BLACK]);

	//Highlight selected square
	if(hltdSquare != -2 && hltdSquare != -1){
		if(currentPlayer == 0){ DrawSquare( locMap[posTable_0[hltdSquare]].x, locMap[posTable_0[hltdSquare]].y, gridSize, colors[HOT_PINK]); }
		else if(currentPlayer == 1){ DrawSquare( locMap[posTable_1[hltdSquare]].x, locMap[posTable_1[hltdSquare]].y, gridSize, colors[HOT_PINK]); }
		else if(currentPlayer == 2){ DrawSquare( locMap[posTable_2[hltdSquare]].x, locMap[posTable_2[hltdSquare]].y, gridSize, colors[HOT_PINK]);}
		else if(currentPlayer == 3){ DrawSquare( locMap[posTable_3[hltdSquare]].x, locMap[posTable_3[hltdSquare]].y, gridSize, colors[HOT_PINK]);}
	}
	
	//Draw pawns
	for(int i = 0; i < 16; ++i){
		int player = 0;
		if(i > 3) { player = 1; }
		if(i > 7) { player = 2; }
		if(i > 11) { player = 3; }
		
		// Draw Pawn in jail
		if(pawnPosition[i] == PAWN_IN_JAIL) {
			if(hltdSquare == -1 && player == currentPlayer) {
				DrawSquare(homeSquareOffset[player].x, homeSquareOffset[player].y, gridSize, colors[HOT_PINK]);
			} else {
				DrawSquare(homeSquareOffset[player].x, homeSquareOffset[player].y, gridSize, colors[WHITE]);
			}
			DrawPawn(homeSquareOffset[player].x, homeSquareOffset[player].y, player);
			
		} else if (pawnPosition[i] == PAWN_HOME) {
			// DO NOT DRAW ANYTHING

		} else {
			if(player == 0){ DrawPawn(locMap[posTable_0[pawnPosition[i]]].x, locMap[posTable_0[pawnPosition[i]]].y, player); }
			else if(player == 1){ DrawPawn(locMap[posTable_1[pawnPosition[i]]].x, locMap[posTable_1[pawnPosition[i]]].y, player); }
			else if(player == 2){ DrawPawn(locMap[posTable_2[pawnPosition[i]]].x, locMap[posTable_2[pawnPosition[i]]].y, player); }
			else if(player == 3){ DrawPawn(locMap[posTable_3[pawnPosition[i]]].x, locMap[posTable_3[pawnPosition[i]]].y, player); }
		}
		// Print number of pawns in jail
		char strJail[1];
		sprintf(str, "%d", pawnsInJail[player]);
		DrawString(homeSquareOffset[player].x, homeSquareOffset[player].y, str, colors[BLACK]);
	}

	if(firstRun == 1){
		firstRun = 0;
	}
	
	glutSwapBuffers(); // do not modify this line..

}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
 * is pressed from the keyboard
 *
 * You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
 *
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 *
 * */

void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {

	} else if (key == GLUT_KEY_RIGHT) {

	} else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {

	}

	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {

	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	 * this function*/

	glutPostRedisplay();

}

/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
 * is pressed from the keyboard
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 * */
void PrintableKeys(unsigned char key, int x, int y) {
	if (key == 27/* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}

	if (key == 'b' || key == 'B') {
		CheckValidMoves(currentRoll[0]);
		int hasValidMoves = 0;
		for(int i =0; i < 4; ++i){
			if(validMoves[i] != 0){
				hasValidMoves = 1;
			}
		}

		if(hasValidMoves == 1){
			//highlight valid choices; continue to next player if no valid choices
			if(selectedMove < 4){
				selectedMove += 1;
			} else {
				selectedMove = 0;
			}
			if(validMoves[selectedMove] != 0){
				hltdSquare = validMoves[selectedMove];
			}
		}

		// no more valid moves
		if(selectedMove == -1){
			hltdSquare = -2;
			CyclePlayer();
			RollDice();
		}
	}
	
	if (key == 32) { //for space
		if(selectedMove != -1){
			MovePawn();
			selectedMove = -1;
		}
	}

	if (key == 'r' || key == 'R') {
		resetGame = 1;
	}

	glutPostRedisplay();
}

/*
 * This function is called after every 1000.0/FPS milliseconds
 * (FPS is defined on in the beginning).
 * You can use this function to animate objects and control the
 * speed of different moving objects by varying the constant FPS.
 *
 * */
void Timer(int m) {
	// implement your functionality here

	// once again we tell the library to call our Timer function after next 1000/FPS
	glutTimerFunc(1000.0, Timer, 0);
}

/*This function is called (automatically) whenever your mouse moves witin inside the game window
 *
 * You will have to add the necessary code here for finding the direction of shooting
 *
 * This function has two arguments: x & y that tells the coordinate of current position of move mouse
 *
 * */
void MousePressedAndMoved(int x, int y) {
	cout << x << " " << y << endl;
	glutPostRedisplay();
}
void MouseMoved(int x, int y) {

	glutPostRedisplay();
}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
 *
 * You will have to add the necessary code here for shooting, etc.
 *
 * This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
 * x & y that tells the coordinate of current position of move mouse
 *
 * */
void MouseClicked(int button, int state, int x, int y) {
	// TODO: divide into areas; check if area is clicked; check if any pawn in the area; move pawn; redraw
	if (button == GLUT_LEFT_BUTTON) {
		if (state==GLUT_UP){
			cout << "x:" << x << " y:" << y << endl;
		}

	} else if (button == GLUT_RIGHT_BUTTON) {

	}
	glutPostRedisplay();
}

/*
 * our gateway main function
 * */
int main(int argc, char*argv[]) {

	//b = new Board(60, 60); // create a new board object to use in the Display Function ...

	int width = 975, height = 975; // i have set my window size to be 800 x 600
	//b->InitalizeBoard(width, height);
	InitRandomizer(); // seed the random number generator...
	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("PF Project"); // set the title of our game window
	SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	//glutDisplayFunc(display); // tell library which function to call for drawing Canvas.

	glutDisplayFunc(GameDisplay); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0, Timer, 0);

	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse
	glutMotionFunc(MousePressedAndMoved); // Mouse

	// now handle the control to library and it will call our registered functions when
	// it deems necessary...
	glutMainLoop();
	return 1;
}
#endif /* AsteroidS_CPP_ */
