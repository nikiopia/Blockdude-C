#ifndef BLOCKDUDE_H
#define BLOCKDUDE_H

// INCLUDES
#include <stdio.h> //printf,file io
#include <stdlib.h> //system
#include <conio.h> //getch

// DEFINES
#define SCREEN_W_MAX 	51
#define SCREEN_H_MAX 	20
#define WORLD_COUNT 	1

// LIST OF WORLD FILES
const char worldList[1][20] = {
	"dev0.world"
};

// LIST OF WHITELISTED CHARACTERS
const char okChars[] = "#B D<>";

// TYPEDEFS
typedef struct gameState {
	char screen[SCREEN_H_MAX][SCREEN_W_MAX];
	int screenHeight;
	int playerX;
	int playerY;
	int holdingBox;
	int gameRunning;
	int updatedScreen;
} Game;

// FUNCTION PROTOTYPES
/*!
 *	@function	gameInit
 *	@param		gameObj		Pointer to the game object
 *	@result		Initializes the game object
**/
void gameInit(Game* gameObj);

/*!
 *	@function	loadWorld
 *	@param		gameObj		Pointer to the game object
 *	@param		worldIndex	Index of the world file in worldList to loadWorld
 *	@result		Loads the world data into the game object
**/
void loadWorld(Game* gameObj, int worldIndex);

/*!
 * 	@function	printWorld
 *	@param		gameObj		Pointer to the game object
 *	@param		debug		Defaults to 0 -> cls before print, 1 -> no cls
 *	@result		Prints the world on-screen
**/
void printWorld(Game* gameObj, int debug);

/*!
 *	@function 	moveHandler
 *	@param		gameObj		Pointer to the game object
 *	@param		direction	Direction to move (-1 -> left, 1 -> right)
 *	@result		Move the character, update its posisition and refresh the screen 
**/
void moveHandler(Game* gameObj, int direction);

#endif