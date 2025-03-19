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
	beingToGiveLife->hitpoints = 5;
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
}


void beingToPrint(const Being *beingToPrint)
{
	init_pair(beingToPrint->myColor, beingToPrint->myColor,-1);  // print color according to being property
	attron(COLOR_PAIR(beingToPrint->myColor));
	mvprintw(beingToPrint->posy,beingToPrint->posx,"*");
	attroff(COLOR_PAIR(beingToPrint->myColor));
}


int spawnBeing(Being *beingToGiveLife, const int *beingNbr, const MyColor *myColor)
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
	setBeingDefaults(beingToGiveLife,&testx,&testy,myColor);

	// Initial placing of being.
	beingToPrint(beingToGiveLife);

	// return false if position is not found
	if(positionFreeTest >= 10000000)
		return 0;
	else
		return 1;
}


void movement(Being *beingToTurn)
{
	// only move if not resting or fighting
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


void turnBeing(Being *beingToTurn, const int *beingNbr)
{
	//Erase old position
	mvprintw(beingToTurn->posy,beingToTurn->posx, " ");

	// Evaluate and choose
	decision(beingToTurn);

	//Move according to decision.
	movement(beingToTurn);

	// Print new position.
	beingToPrint(beingToTurn);
}


void strikeBeingOnCoordinates(const int *x, const int *y){
	if(getRndNum(3)==3){
		beingToStrike->hitpoints--;
		attron(COLOR_PAIR(beingToPrint->myColor));
		mvprintw(beingToStrike->posy,beingToStrike->posx,"*");
		attroff(COLOR_PAIR(beingToPrint->myColor));
	}
}


//bool isEnemy(Being *beingPerspective, const int *targetx, const int *targety)
//{
//	char targetobject;
//	char targetcolor;
//	targetobject = mvinch(*targety,*targetx) & A_CHARTEXT;  // Get target object.
//	targetcolor = mvinch(*targety,*targetx) & A_COLOR;  	// Get target color.

	// Friend or foe
//	if(targetobject == '*' && targetcolor != beingPerspective->myColor)  //being with different color
//		return TRUE;
//	else
//		return FALSE;
//}
