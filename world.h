#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>
#include "being.h"

extern int maxx;
extern int maxy;


/* Function prototypes */

void placeObstacles();
void buildWorld();
unsigned int getMaxNbrOfBeings();
unsigned int getNbrOfBeings();
unsigned int spawnBeings(Being *beings, const unsigned int *nbrOfBeings);
int setSimulationSpeed();
void runWorld();
bool checkIfCoordinatesAreClear(const int *x, const int *y);


#endif

