#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int getRndNum(const int nmbOfTurnouts) // Function to return a random number.
{       
	int rand_nbr = (rand()%nmbOfTurnouts+1); 
	return rand_nbr;
}
