#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "zorc.h"
#include "main.h"
#include "player.h"
#include "command.h"
#include "room.h"
#include "util.h"

void initCommands(struct game *game)
{
	int size = 14;
	game->cmds = malloc(sizeof(struct command)*size);
	game->cmdsize = size;
	game->cmds[0] = getCommand("help", visible, &printCommands);
	game->cmds[1] = getCommand("look", visible, &printCommandDummy);
	game->cmds[2] = getCommand("open", visible, &commandOpenDoor);
	game->cmds[3] = getCommand("show", visible, &printCommandDummy);
	game->cmds[4] = getCommand("exit", visible, &exitMe);
	game->cmds[5] = getCommand("unlock", visible, &commandUnlockDoor);
	game->cmds[6] = getCommand("hit", visible, &printCommandDummy);
	game->cmds[7] = getCommand("equip", visible, &printCommandDummy);
	game->cmds[8] = getCommand("unequip", visible, &printCommandDummy);
	game->cmds[9] = getCommand("whoprogrammedme", hidden, &printMe);
	game->cmds[10] = getCommand("whowroteme", hidden, &printMe);
	game->cmds[11] = getCommand("attack", visible, &printCommandDummy);
	game->cmds[12] = getCommand("turn", visible, &commandTurnMe);
	game->cmds[13] = getCommand("go", visible, &commandGoDirection);
}

struct command getCommand(const char name[21], int hidden, void *fp)
{
	struct command command;
	strcpy(command.name, name);
	command.hidden = hidden;
	command.fp = fp;
	return command;
}
 

// What a mess...
void promptCommand(struct game *game)
{
	char line[256];
	char **args = NULL;
	fgets(line, sizeof(line), stdin);
	char *cmd = strtok(line, " \t");
	char *subarg = NULL;
	unsigned int arg = 0;

	for(;;)
	{
		// dangerous realloc in case of NULL pointer, should use a new
		// pointer, then assign that pointer to args once it passes and if not NULL.
		args = realloc(args, sizeof (*args) * (arg));
		subarg = strtok(NULL, " \t");

		
		if(!subarg)
		{
			break;
		}
		// remove trail and to lowercase
		subarg[strcspn(subarg, "\n")] = '\0';
		args[arg++] = toLowerCase(subarg, sizeof subarg/ sizeof *subarg);
	}
	// remove trailing newline which fgets leaves in..
	cmd[strcspn(cmd, "\n")] = '\0';
	struct command *c = parseCommand(game, cmd, sizeof cmd/sizeof *cmd);
	if(c != NULL)
	{
		c->fp(game, args, arg);
		//
		printf("\n");
	}
	else
	{
		printf("I don't know what %s is!\n", cmd);
	}
	// free and possibly clear buffer...
	free(args);
}

struct command* parseCommand(struct game *game, char *com, int size)
{
	struct command *ret = NULL;
	com = toLowerCase(com, size);
	for(int i = 0; i < game->cmdsize; i++)
	{
		if(strcmp(com, game->cmds[i].name) == 0)
		{
			ret = &game->cmds[i];
		}
	}
	return ret;
}

void printCommands(const struct game *game)
{
	printf("\n** The following commands are available to me **\n\n");
	for(int i = 0; i < game->cmdsize; i++)
	{
		if(!game->cmds[i].hidden)
		{
			printf("> %s\n",game->cmds[i].name);
		}
	}
	printf("\n");
}

void exitMe(struct game *game)
{
	destroyGame(game);
	isRunning = 0;
	exit(0);
}

void errAbort(char *err)
{
	printf(err);
	exit(0);
}

void printCommandDummy(void *p, char **args, int arg)
{
	printf("Command Dummy!\n");
	for(int i = 0; i < arg; i++)
	{
		printf("> %s\n", args[i]);
	}
}

// What a mess ...
void commandTurnMe(void *p, char **args, int arg)
{
	struct game *game = (struct game *)p;
	if(arg == 0)
	{
		printf("Ehh..turn to...what?\n");
		return;
	}
	setPlayerDirection(game, args, arg);
}

void commandOpenDoor(void *p, char **args, int arg)
{
	struct game *game = (struct game *)p;
	int direction = game->zorc->direction;
	if(arg == 0)
	{
		printf("Ehh...what door should I open exactly?\n");
		return;
	}

	if(game->zorc->rm->trans[direction] != NULL)
	{
		if(strcmp(args[0], game->zorc->rm->trans[direction]->name) == 0)
		{
			if(game->zorc->rm->trans[direction]->isLocked)
			{
				printf("The door seems to be locked!\n");
				return;
			}
			game->zorc->rm = game->zorc->rm->trans[direction]->nxt;
			printRoom(game);
			return;
		}
		else
		{
			printf("I don't know how to open %s!\n", args[0]);
			return;
		}
	}
	printf("Ehh...is there even a door in that direction?\n");
}

void commandUnlockDoor(void *p, char **args, int arg)
{
	struct game *game = (struct game *)p;
	int direction = game->zorc->direction;

	if(arg == 0)
	{
		printf("Ehh...what door should I unlock exactly?\n");
		return;
	}

	if(game->zorc->rm->trans[direction] != NULL)
	{
		if(strcmp(args[0], game->zorc->rm->trans[direction]->name) == 0)
		{
			if(game->zorc->rm->trans[direction]->isLocked)
			{
				for(int i = 0 ; i < game->zorc->keysize; i++)
				{
					if(&game->zorc->keys[i] != NULL)
					{
						if(game->zorc->keys[i].code == game->zorc->rm->trans[direction]->code)
						{
							printf("Door unlocked!\n");
							return;
						}
					}
				}	
			} 
			else
			{
				printf("This door doesn't seem to be locked!\n");
				return;
			}
		}
		else
		{
			printf("I don't know how to open %s!\n", args[0]);
			return;
		}
	}
	else
	{
		printf("Ehh...is there even a door in that direction?\n");
		return;
	}
}

void commandGoDirection(void *p, char **args, int arg)
{
	struct game *game = (struct game *)p;
	char **over = malloc(sizeof *over);
	over[0] = "door";
	setPlayerDirection(game, args, arg);
	commandOpenDoor(p, over, 1);
	free(over);
}
