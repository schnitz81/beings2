#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "being.h"
#include "world.h"
#include "ai.h"
#include "event.h"

void setBeingDefaults(Being *beingToGiveLife, const int *x, const int *y, const MyColor *myColor)
{
	// Initialize values of new being.
	beingToGiveLife->posx = *x;
	beingToGiveLife->posy = *y;
	beingToGiveLife->hitpoints = 3;
	beingToGiveLife->myHeading = getRndNum(8)-1;
	beingToGiveLife->obstacles.leftfar = 0;
	beingToGiveLife->obstacles.leftnear = 0;
	beingToGiveLife->obstacles.middlefar = 0;
	beingToGiveLife->obstacles.middlenear = 0;
	beingToGiveLife->obstacles.rightfar = 0;
	beingToGiveLife->obstacles.rightnear = 0;
	beingToGiveLife->resting = TRUE;
	beingToGiveLife->myColor = *myColor;
	beingToGiveLife->fighting = FALSE;
	beingToGiveLife->isHit = FALSE;
	beingToGiveLife->next = NULL;
}


void beingToPrint(const Being *beingToPrint)
{
	int beingColor;
	if(beingToPrint->fighting)  // make being cyan if attacking
		beingColor = COLOR_CYAN;
	else
		beingColor = beingToPrint->myColor;
	init_pair(beingColor,beingColor,-1);  // print color according to being property
	attron(COLOR_PAIR(beingColor));
	mvprintw(beingToPrint->posy,beingToPrint->posx,"*");
	attroff(COLOR_PAIR(beingColor));
}


void spawnBeing(Being *beingsCursor, const MyColor *myColor)
{
	//choose coordinate without obstacle or other being
	bool coordinateIsClear = FALSE;
	int testx;
	int testy;
	unsigned long int positionFreeTest = 0;

	while(!coordinateIsClear && positionFreeTest < 10000000){
		positionFreeTest++;
		testx = getRndNum(maxx-1);
		testy = getRndNum(maxy-1);
		coordinateIsClear = checkIfCoordinatesAreClear(&testx, &testy);
	}

	// Initial placing of being.
	if(coordinateIsClear){

		Being *beingToGiveLife = malloc(sizeof(Being));
		if(beingToGiveLife == NULL){
			endwin();
			printf("\nError creating a new being.\n");
			exit(1);
		}
		setBeingDefaults(beingToGiveLife,&testx,&testy,myColor);

		// find last being in linked list
		if(beingsCursor!=NULL){
			while(beingsCursor->next != NULL)
				beingsCursor = beingsCursor->next;
			beingsCursor->next = beingToGiveLife;  // add new being in the end of the list
		}
		else  // if first being
			beingsCursor = beingToGiveLife;

		beingToPrint(beingToGiveLife);
	}
	else{  // if no place to spawn being is found
		endwin();
		printf("\nError: No space for all beings to spawn. Try a lower number or make the terminal window larger.\n");
		exit(1);
	}
}


void movement(Being *beingToTurn)
{
	// only move if alive and not resting or fighting
	if(!beingToTurn->resting && !beingToTurn->fighting){
		switch(beingToTurn->myHeading){
			case UP:
				beingToTurn->posy--;
				break;
			case UPRIGHT:
				beingToTurn->posy--;
				beingToTurn->posx++;
				break;
		 	case RIGHT:
				beingToTurn->posx++;
				break;
			case DOWNRIGHT:
				beingToTurn->posy++;
				beingToTurn->posx++;
				break;
			case DOWN:
				beingToTurn->posy++;
				break;
			case DOWNLEFT:
				beingToTurn->posy++;
				beingToTurn->posx--;
				break;
			case LEFT:
				beingToTurn->posx--;
				break;
			case UPLEFT:
				beingToTurn->posy--;
				beingToTurn->posx--;
				break;
			default:
				break;
		}
	}
}


void turnBeing(Being *beingToTurn, Attackposition *attackposition)
{
	if(beingToTurn->isHit){  // skip turn if hit (stunned for 1 turn)
		beingToTurn->isHit = FALSE;
		return;
	}

	// reset fighting mode every turn, will be reenabled when needed
	beingToTurn->fighting = FALSE;

	//Erase old position
	mvprintw(beingToTurn->posy,beingToTurn->posx, " ");

	// Evaluate and choose
	decision(beingToTurn, attackposition);

	//Move according to decision.
	movement(beingToTurn);

	// Print new position.
	beingToPrint(beingToTurn);
}
