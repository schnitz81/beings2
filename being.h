#ifndef BEING_H
#define BEING_H


typedef struct attack_coordinates{
	int posy;
	int posx;
} Attackposition;


typedef enum heading { UNDERVALUE2 = -2, UNDERVALUE1, UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT, UPLEFT, OVERVALUE1, OVERVALUE2 } Heading;
typedef enum object { NONE, TEAMBEING, FIGHTINGBEING, ENEMYBEING, DAMAGEDBEING, DEADBEING, FENCE } Object;
typedef enum myColor { GREEN = 2, YELLOW = 3 } MyColor;
//typedef enum fightmode { NONVIOLENT, FIGHTING } Fightmode; 
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
	//bool alive;
	Heading myHeading;
	//Sight peek;
	Sight obstacles;
	bool resting;
	MyColor myColor;
	bool fighting;
	bool isHit;
	struct being *next;
} Being;


/* Function prototypes */

void setBeingDefaults(Being *beingToGiveLife, const int *x, const int *y, const MyColor *myColor);
void beingToPrint(const Being *beingToPrint);
void spawnBeing(Being *beingsCursor, const MyColor *myColor);
void movement(Being *beingToTurn);
void turnBeing(Being *beingToTurn, Attackposition *attackposition);
//bool isEnemy(Being *beingMe, const int *targetx, const int *targety);

#endif

