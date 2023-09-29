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
	int screenWidth;
	int playerX;
	int playerY;
	int holdingBox;
	int gameRunning;
	int updatedScreen;
	int levelWon;
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

/*!
 *	@function	posClear
 *	@param		gameObj		Pointer to the game object
 *	@param		x			X location of test
 *	@param		y			Y location of test
 *	@result		Returns 1 if area is clear or a door, 0 if else
**/
int posClear(Game* gameObj, int x, int y);

/*!
 *	@function	movePlayer
 *	@param		gameObj		Pointer to the game object
 *	@param		destX		X location of destination
 *	@param		destY		Y location of destination
 *	@result		Moves the player and updates relevant info
**/
void movePlayer(Game* gameObj, int destX, int destY);

/*!
 *	@function	blockInteract
 *	@param		gameObj		Pointer to the game object
 *	@param		mode		Mode for dealing with blocks (1 -> pick up, -1 -> put down)
 *	@result		Picking up and putting down blocks
**/
void blockInteract(Game* gameObj, int mode);

#endif