// INCLUDES
#include "blockdude.h"

// FUNCTION DEFINITIONS
void gameInit(Game* gameObj) {
	gameObj->gameRunning = 1; // For the game loop
	gameObj->updatedScreen = 1;
	
	// Init player coords with placeholders
	gameObj->playerX = -1;
	gameObj->playerY = -1;
	gameObj->holdingBox = 0;
	
	// Init screenHeight as full size
	gameObj->screenHeight = SCREEN_H_MAX;
	
	// Init screen with filler characters
	for (int y = 0; y < SCREEN_H_MAX; ++y) {
		for (int x = 0; x < (SCREEN_W_MAX - 1); ++x) {
			// Doesn't loop over full array to leave room for null terminators
			gameObj->screen[y][x] = ' '; // The filler character
		}
	}
	// Add null terminators
	for (int y = 0; y < SCREEN_H_MAX; ++y) {
		gameObj->screen[y][SCREEN_W_MAX - 1] = '\0'; // The null terminator
	}
	
	// Check existence (not content) of world files in worldList
	FILE* file_ptr;
	int success = 1; // To keep status of checks
	for (int i = 0; i < WORLD_COUNT; ++i) {
		file_ptr = fopen(worldList[i], "r");
		
		if (!file_ptr) {
			success = 0;
			printf("gameInit(): Missing world file: %s\n",worldList[i]);
		} else {
			fclose(file_ptr); // Have to be carefull of fclose(nullptr);
		}
	}
	if (!success) {
		gameObj->gameRunning = 0;
		return;
	}
	
	return;
}

void loadWorld(Game* gameObj, int worldIndex) {
	// Check for proper index
	if (worldIndex < 0 || worldIndex >= WORLD_COUNT) {
		printf("loadWorld(): worldIndex out of range\n");
		gameObj->gameRunning = 0;
		return;
	}
	
	// Load file
	FILE* file_ptr = fopen(worldList[worldIndex], "r");
	
	if (!file_ptr) {
		printf("loadWorld(): Couldn't open world file: %s\n",worldList[worldIndex]);
		gameObj->gameRunning = 0;
		return;
	}
	
	char line[SCREEN_W_MAX];
	char* i; // Allows looping over characters in a string one by one
	int y = 0; // The current on-screen y index to write to
	int charPassed; // For checking whether a character is valid and can be shown
	while (fgets(line, (SCREEN_W_MAX - 1), file_ptr) && y < SCREEN_H_MAX) {
		i = &line[0];
		while (*i != '\0' && *i != '\n' && (i - &line[0] + 1) < (SCREEN_W_MAX - 1)) {
			charPassed = 0;
			for (int j = 0; j < 6; ++j) {
				charPassed = charPassed || *i == okChars[j];
			}
			if (charPassed) {
				gameObj->screen[y][i - &line[0]] = *i;
			}
			
			if (*i == '<' || *i == '>') {
				// Save player location
				gameObj->playerX = (i - &line[0]);
				gameObj->playerY = y;
			}
			i++;
		}
		y++;
	}
	printf("\n");
	fclose(file_ptr);
	
	// Has the player been found check
	if (gameObj->playerX < 0) {
		printf("loadWorld() in '%s', cannot find player\n",worldList[worldIndex]);
		gameObj->gameRunning = 0;
		return;
	}
	return;
}

void printWorld(Game* gameObj, int debug) {
	if (!debug) { system("cls"); }
	for (int y = 0; (y < SCREEN_H_MAX && y < gameObj->screenHeight); ++y) {
		printf("|%s|\n",gameObj->screen[y]);
	}
}

void moveHandler(Game* gameObj, int direction) {
	if (direction != -1 && direction != 1) {
		printf("moveHandler(): direction isn't -1 or 1\n");
		gameObj->gameRunning = 0;
		return;
	}
	// Local copy of the coordinates
	int tPlayerX = gameObj->playerX;
	int tPlayerY = gameObj->playerY;
	if (tPlayerX < 0 || tPlayerY < 0 || tPlayerX >= SCREEN_W_MAX ||
		tPlayerY > SCREEN_H_MAX) {

		printf("moveHandler(): player position out of bounds\n");
		gameObj->gameRunning = 0;
		return;
	}
	if (gameObj->screen[tPlayerY][tPlayerX] != '<' &&
	gameObj->screen[tPlayerY][tPlayerX] != '>') {

		printf("moveHandler(): player coordinates incorrect\n");
		gameObj->gameRunning = 0;
		return;
	}
	
	// Getting to here guarantees direction's value is good,
	// the player is in-bounds, and they're where they should be
	
	// Handle turning cases
	if (gameObj->screen[tPlayerY][tPlayerX] == '<' && direction == 1) {
		gameObj->screen[tPlayerY][tPlayerX] = '>';
		gameObj->updatedScreen = 1;
		return;
	}
	if (gameObj->screen[tPlayerY][tPlayerX] == '>' && direction == -1) {
		gameObj->screen[tPlayerY][tPlayerX] = '<';
		gameObj->updatedScreen = 1;
		return;
	}
	
	// Must be time to really move
	// See if it's at least possible to run the movement checks
	int moveCheckX = tPlayerX + direction;
	if (moveCheckX < 0 || moveCheckX >= SCREEN_W_MAX) { return; }
	
	if (gameObj->holdingBox) {
		
	} else {
		
	}
}

// MAIN
int main(int argc, char* argv[]) {
	// Initialize everything
	Game gameObj;
	gameInit(&gameObj);
	
	// Load level 0
	loadWorld(&gameObj, 0);
	
	char key;
	int keyCode;
	int arrowCode;
	while (gameObj.gameRunning) {
		if (gameObj.updatedScreen) {
			gameObj.updatedScreen = 0;
			printWorld(&gameObj, 0);
		}
		
		// Get keypress without requiring enter being pressed
		key = getch();
		keyCode = (*(int*) &key) & 0xFF;
		arrowCode = -1;
		// User hit escape key so stop
		if (keyCode == 27) { gameObj.gameRunning = 0; }
		if (keyCode == 224) { arrowCode = getch(); }
		switch (arrowCode) {
			case 75:
				moveHandler(&gameObj, -1); // Left arrow
				break;
			case 77:
				moveHandler(&gameObj, 1); // Right arrow
				break;
			case 72:
				printf("Up arrow\n"); //blockInteract(&gameObj, 1); } // Up arrow
				break;
			case 80:
				printf("Down arrow\n"); //blockInteract(&gameObj, -1); } // Down arrow
				break;
			default:
		}
	}
	
	return 0;
}