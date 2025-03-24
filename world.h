#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>
#include "being.h"


typedef enum gamemode {FREEROAM, REGROUP, ATTACK} Gamemode;


extern Gamemode gamemode;
extern int maxy;
extern int maxx;


/* Function prototypes */

void placeObstacles();
void buildWorld();
unsigned int getMaxNbrOfBeings();
unsigned int getNbrOfBeings();
unsigned int spawnBeings(Being *beings, const unsigned int *nbrOfBeings, const MyColor *beingColor);
int setSimulationSpeed();
void hitHandleBeing(Being *beingToTurn, Attackposition *attackposition);
void runWorld();
bool checkIfCoordinatesAreClear(const int *x, const int *y);


#endif
