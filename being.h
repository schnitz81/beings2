#ifndef BEING_H
#define BEING_H

typedef enum heading { UNDERVALUE2 = -2, UNDERVALUE1, UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT, UPLEFT, OVERVALUE1, OVERVALUE2 } Heading;
typedef enum object { NONE, TEAMBEING, ENEMYBEING, FENCE } Object;
typedef enum myColor { GREEN = 2, BLUE = 4 } MyColor;
typedef enum fightevent { NONVIOLENT, STALKING, STRIKING, DAMAGETAKING } Fightevent; 
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
	int hitpoints;
	Heading myHeading;
	//Sight peek;
	Sight obstacles;
	bool resting;
	MyColor myColor;
	Fightevent fightevent;
} Being;


/* Function prototypes */

void setBeingDefaults(Being *beingToGiveLife, const int *x, const int *y, const MyColor *myColor);
void beingToPrint(const Being *beingToPrint);
int spawnBeing(Being *beingToGiveLife, const int *beingNbr, const MyColor *myColor);
void movement(Being *beingToTurn);
void turnBeing(Being *beingToTurn, const int *beingNbr);
bool isEnemy(Being *beingMe, const int *targetx, const int *targety);

#endif

