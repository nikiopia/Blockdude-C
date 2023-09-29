// INCLUDES
#include "blockdude.h"

// FUNCTION DEFINITIONS
void gameInit(Game* gameObj) {
	if (!gameObj) { return; }
	
	gameObj->gameRunning = 1; // For the game loop
	gameObj->updatedScreen = 1;
	gameObj->levelWon = 0;
	
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
	if (!gameObj) { return; }
	
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
	int doorFound = 0;
	int blankLine;
	while (fgets(line, (SCREEN_W_MAX - 1), file_ptr) && y < SCREEN_H_MAX) {
		i = &line[0];
		blankLine = 1;
		while (*i != '\0' && *i != '\n' && (i - &line[0] + 1) < (SCREEN_W_MAX - 1)) {
			charPassed = 0;
			for (int j = 0; j < 6; ++j) {
				charPassed = charPassed || *i == okChars[j];
			}
			if (charPassed) {
				gameObj->screen[y][i - &line[0]] = *i;
				blankLine = 0;
			}
			
			if (*i == '<' || *i == '>') {
				// Save player location
				gameObj->playerX = (i - &line[0]);
				gameObj->playerY = y;
			}
			
			if (*i == 'D') {
				doorFound = 1;
			}
			i++;
		}
		if (blankLine) {
			break;
		}
		y++;
	}
	printf("\n");
	fclose(file_ptr);
	gameObj->screenHeight = y + 1;
	
	// Has the player been found check
	if (gameObj->playerX < 0) {
		printf("loadWorld() in '%s', couldn't find player\n",worldList[worldIndex]);
		gameObj->gameRunning = 0;
	}
	
	// Has the door been found?
	if (!doorFound) {
		printf("loadWorld() in '%s', couldn't find a door\n",worldList[worldIndex]);
		gameObj->gameRunning = 0;
	}
	
	gameObj->updatedScreen = 1;
}

void printWorld(Game* gameObj, int debug) {
	if (!gameObj) { return; }
	if (!debug) { system("cls"); }
	
	int tScreenHeight = gameObj->screenHeight - 1;
	for (int y = 0; (y < SCREEN_H_MAX && y < tScreenHeight); ++y) {
		printf("|%s|\n",gameObj->screen[y]);
	}
}

void moveHandler(Game* gameObj, int direction) {
	if (!gameObj) { return; }
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
	char player = gameObj->screen[tPlayerY][tPlayerX];
	if (player != '<' && player != '>') {
		printf("moveHandler(): player coordinates incorrect\n");
		gameObj->gameRunning = 0;
		return;
	}
	
	// Getting to here guarantees direction's value is good,
	// the player is in-bounds, and they're where they should be
	
	// Handle turning cases
	
	if (player == '<' && direction == 1) {
		gameObj->screen[tPlayerY][tPlayerX] = '>';
		gameObj->updatedScreen = 1;
		return;
	}
	if (player == '>' && direction == -1) {
		gameObj->screen[tPlayerY][tPlayerX] = '<';
		gameObj->updatedScreen = 1;
		return;
	}
	
	// Time to get moving babyyyyy
	// See if it's at least possible to run the movement checks
	int moveCheckX = tPlayerX + direction;
	if (moveCheckX < 0 || moveCheckX >= SCREEN_W_MAX || tPlayerY < 2) { return; }
	// Is the player too close to the ceiling?
	// This guarantees getting a character from y=(tPlayerY - 2) is possible
	
	// If player is holding a box, verify the box's existence
	int holdingBox = gameObj->holdingBox;
	if (holdingBox) {
		gameObj->holdingBox = gameObj->screen[tPlayerY - 1][tPlayerX] == 'B';
	}
	
	/* Compute the deciders for moving once, pos key:
	 [3]
	 [2]
	>[1]
	*/
	int pos1 = posClear(gameObj, moveCheckX, tPlayerY);
	int pos2 = posClear(gameObj, moveCheckX, tPlayerY - 1);
	int pos3 = posClear(gameObj, moveCheckX, tPlayerY - 2);
	// Movement fail case for not holding a box, before ||
	// Movement fail case for holding a box, after ||
	if ((!holdingBox && !pos1 && !pos2) ||
		(holdingBox && !pos1 && !pos2 && !pos3)) { return; }
	
	// Diagonal upward move case (with box), before ||
	// Diagonal upward move case (no box), after ||
	if ((holdingBox && !pos1 && pos2 && pos3) ||
		(!holdingBox && !pos1 && pos2)) {
		movePlayer(gameObj, moveCheckX, tPlayerY - 1);
		return;
	}
	
	// Floor search timeeee
	while (tPlayerY < SCREEN_H_MAX &&
		posClear(gameObj, moveCheckX, tPlayerY)) {
		
		tPlayerY++; // This moves the search downward on-screen
	}
	movePlayer(gameObj, moveCheckX, tPlayerY - 1);
}

int posClear(Game* gameObj, int x, int y) {
	if (!gameObj) { return 0; }
	if (x < 0 || x >= SCREEN_W_MAX) { return 0; }
	if (y < 0 || y > SCREEN_H_MAX) { return 0; }
	
	char testChar = gameObj->screen[y][x];
	return (testChar == ' ' || testChar == 'D');
}

void movePlayer(Game* gameObj, int destX, int destY) {
	if (!gameObj) { return; }
	if (destX < 0 || destX >= SCREEN_W_MAX) { return; }
	if (destY < 2 || destY > SCREEN_H_MAX) { return; } // Guarantees safety w/ boxes
	
	// Save player and remove the old position with the box (if applicable)
	char player = gameObj->screen[gameObj->playerY][gameObj->playerX];
	gameObj->screen[gameObj->playerY][gameObj->playerX] = ' ';
	// Place them in the new spot
	if (gameObj->screen[destY][destX] == 'D') {
		gameObj->levelWon = 1;
	}
	gameObj->screen[destY][destX] = player;
	if (gameObj->holdingBox) {
		gameObj->screen[gameObj->playerY - 1][gameObj->playerX] = ' ';
		gameObj->screen[destY - 1][destX] = 'B';
	}
	gameObj->playerX = destX;
	gameObj->playerY = destY;
	gameObj->updatedScreen = 1;
}

void blockInteract(Game* gameObj, int mode) {
	if (!gameObj) { return; }
	if (mode != -1 && mode != 1) {
		printf("blockInteract(): mode isn't -1 or 1\n");
		gameObj->gameRunning = 0;
		return;
	}
	int tPlayerX = gameObj->playerX;
	int tPlayerY = gameObj->playerY;
	
	// Verify player at coordinates
	int direction = 0;
	char player = gameObj->screen[tPlayerY][tPlayerX];
	if (player != '>' && player != '<') {
		printf("blockInteract(): player coordinates incorrect\n");
		gameObj->gameRunning = 0;
		return;
	} else {
		direction = player - 61;
	}
	
	int blockX = tPlayerX + direction;
	if (blockX < 0 || blockX >= SCREEN_W_MAX || tPlayerY < 2 || 
		tPlayerY > SCREEN_H_MAX) { return; }
	
	/* Fetch/compute the deciders for block interaction once, pos key:
	   [3]
	[4][2]
	 > [1]
	*/
	int holdingBox = gameObj->holdingBox;
	int pos1 = gameObj->screen[tPlayerY][blockX] == 'B';
	int pos2 = gameObj->screen[tPlayerY - 1][blockX] == 'B';
	int pos3 = gameObj->screen[tPlayerY - 2][blockX] == 'B';
	int pos4 = gameObj->screen[tPlayerY - 1][tPlayerX] == ' ';
	
	// Fail case for picking up, before ||
	// Fail case for putting down, middle
	// Something overhead, cant pick up, after last ||
	if ((mode == 1 && !pos1 && !pos2 && !pos3) ||
		(mode == -1 && !pos1 && !pos2) ||
		(mode == 1 && !pos4)) { return; }
	
	// Moving pos2 to head
	if (mode == 1 && !pos1 && !pos2) {
		gameObj->holdingBox = 1;
		gameObj->screen[tPlayerY - 1][blockX] = ' ';
		gameObj->screen[tPlayerY - 1][tPlayerX] = 'B';
		return;
	}
	
	// Moving pos1 to head
	if (mode == 1 && !pos1) {
		gameObj->holdingBox = 1;
		gameObj->screen[tPlayerY][blockX] = ' ';
		gameObj->screen[tPlayerY - 1][tPlayerX] = 'B';
		return;
	}
	
	
}

// MAIN
int main(int argc, char* argv[]) {
	// Initialize everything
	Game gameObj;
	gameObj.gameRunning = 0; // In case something goes wrong with gameInit()
	gameInit(&gameObj);
	
	// Load starter level
	int levelIndex = 0;
	loadWorld(&gameObj, levelIndex);
	
	char key;
	int keyCode;
	int arrowCode;
	while (gameObj.gameRunning) {
		if (gameObj.updatedScreen) {
			gameObj.updatedScreen = 0;
			printWorld(&gameObj, 0);
			if (gameObj.levelWon) {
				printf("YOU WON! -- Press C to continue, ESC to exit\n");
			}
		}
		
		// Get keypress without requiring enter being pressed
		key = getch();
		keyCode = (*(int*) &key) & 0xFF;
		arrowCode = -1;
		if (keyCode == 27) { gameObj.gameRunning = 0; } // ESC -> stop
		if (keyCode == 224) { arrowCode = getch(); } // First byte of arrow code
		if (keyCode == 99 && gameObj.levelWon) {
			gameObj.levelWon = 0;
			levelIndex = (levelIndex + 1) % (WORLD_COUNT);
			loadWorld(&gameObj, levelIndex);
		}
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