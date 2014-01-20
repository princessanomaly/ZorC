#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "zorc.h"
#include "room.h"
#include "main.h"
#include "command.h"
#include "player.h"
#include "version.h"
#include "util.h"
#include "cui.h"
int isRunning = 1;

int main(int argc, char *argv[])
{
	struct game *g = initGame();
	g->zorc = initPlayer("Tessa");
	g->zorc->rm = initRoom("spawnroom", "dark");
	printVersion();
	promptYesNoQuestion("When will it be night by day?", "yes", "Hah! A spark of intelligence!", "Hah! You're only good for wielding a sword!");
	initCommands(g);
	printCommands(g);
	while(isRunning)
	{
		promptCommand(g);
	}
	destroyGame(g);
    return 0;
}

struct door* initDoor(const char name[21], int id, int isLocked, int code)
{
	struct door *door = malloc(sizeof(struct door));
	if(door == NULL)
	{
		errAbort("ERROR!");
	}
	strcpy(door->name, name);
	door->code = code;
	door->isLocked = isLocked;
	door->id = id;
	return door;
}

void printZorc(const struct game *g)
{
	struct player *zorc = g->zorc;
	printf("My name is %s.\n", zorc->name);
}

void printRoom(const struct game *g)
{
	struct room *rm = g->zorc->rm;
	printf("I'm in a %s %s.\n", rm->env,rm->name);
}

void printMe(void *p)
{
	printf("Winter's cry comes as a song. Woken by a blade in the dark.\n");
}
