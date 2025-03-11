#ifndef BEING_H
#define BEING_H

typedef enum heading { UNDERVALUE2 = -2, UNDERVALUE1, UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT, UPLEFT, OVERVALUE1, OVERVALUE2 } Heading;
typedef enum object { NONE, OTHERBEING, FENCE } Object;
typedef struct sight {
	 Object leftnear;
	 Object leftfar; 
	 Object middlenear;
	 Object middlefar;
	 Object rightnear;
	 Object rightfar;
} Sight;

typedef struct being{
	int posx;
	int posy;
	Heading myHeading;
	//Sight peek;
	Sight obstacles;
	bool resting;
	int myColor;
} Being;


/* Function prototypes */

void setBeingDefaults(Being *beingToGiveLife, const int *x, const int *y);
void beingToPrint(const Being *beingToPrint);
int spawnBeing(Being *beingToGiveLife, const int *beingNbr);
void movement(Being *beingToTurn);
void turnBeing(Being *beingToTurn, const int *beingNbr);

#endif

