#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "world.h"
#include "event.h"
#include "being.h"

int maxx;
int maxy;

Gamemode gamemode;

void drawOuterWall()
{
	int i;

	// upper frame
	for(i=1;i<maxx-1;i++){
		move(0,i);
		printw("=");
	}

	// lower frame
	for(i=1;i<maxx-1;i++){
		move(maxy-1,i);
		printw("=");
	}

	// left frame
	for(i=0;i<maxy;i++){
		move(i,0);
		printw("|");
	}

	// right frame
	for(i=0;i<maxy;i++){
		move(i,maxx-1);
		printw("|");
	}
	refresh();
}


void placeObstacles()
{
	int i, j, y, x, obstacleDensity = -1;
	bool bLast, bOneMade = FALSE;

	// User chooses obstacle density.
	while(obstacleDensity<0||obstacleDensity>50){
		mvprintw(maxy-1,(maxx/2)-18," Enter obstacle density (0-50):    ");
		getyx(stdscr,y,x);
		move(y,x-4);
		scanw("%d",&obstacleDensity);
	}
	drawOuterWall(); // Redraw outer wall.

	// Exit function if obstacle density is 0.
	if(obstacleDensity==0)
		return;

	// Generate obstacles

	// X-wise obstacles
	for(i=1;i<maxy-1;i++){
		for(j=1;j<maxx-1;j++){
			usleep(60);
			if(bOneMade){  // Always make obstacle longer than one square.
				mvprintw(i,j,"#");
				bOneMade = FALSE;
			}
			else if(bLast && getRndNum(4)!=1)  // Keep making obstacle longer.
				mvprintw(i,j,"#");
			else if(getRndNum(1000/obstacleDensity)==1){  // new obstacle
				mvprintw(i,j,"#");
				bLast = TRUE;
				bOneMade = TRUE;
			}
			else // Obstacle done
				bLast = FALSE;
			refresh();
		}
	}

	// Y-wise obstacles
	for(i=1;i<maxx-1;i++){
		for(j=1;j<maxy-1;j++){
			usleep(60);
			if(bOneMade){
				mvprintw(j,i,"#");
				bOneMade = FALSE;
			}
			else if(bLast && getRndNum(4)!=1)
				mvprintw(j,i,"#");
			else if(getRndNum(1000/obstacleDensity)==1){ // new obstacle
				mvprintw(j,i,"#");
				bLast = TRUE;
				bOneMade = TRUE;
			}
			else // Obstacle done
				bLast = FALSE;
			refresh();
		}
	}
}


void buildWorld()
{
	srand(time(NULL));  // world and actions will be generated randomly.
	initscr();
	curs_set(0);
	start_color();
	use_default_colors();
	getmaxyx(stdscr,maxy,maxx); // set world outer boundaries (global var)
	if(maxy<20 || maxx<40){  // If terminal window is too small.
		endwin();
		printf("\n\nTerminal window too small. Enlarge it and run again.\n\n");
		exit(0);
	}
	drawOuterWall();
	placeObstacles();
}


unsigned int getNbrOfBeings(const MyColor *myColor)
{
	unsigned int nbrOfBeings = 0;
	int y, x;
	unsigned long long int inputNbr = 0;

	while(inputNbr<1||inputNbr>9999999999999){
		mvprintw(maxy-1,(maxx/2)-19," Enter number of ");
		// print color of current selection
		attron(A_BOLD);
		if(*myColor == GREEN)
			printw("green");
		else if(*myColor == BLUE)
			printw("blue");
		attroff(A_BOLD);
		printw(" beings:            ");
		getyx(stdscr,y,x);  // Get current cursor position.
		move(y,x-12);  // Move back cursor three steps.
		scanw("%llu",&inputNbr);
	}
	if(inputNbr>65535)
		nbrOfBeings = 65535;
	else
		nbrOfBeings = inputNbr;
	drawOuterWall(); // Redraw outer wall.
	return nbrOfBeings;
}


int setSimulationSpeed()
{
	int simulationSpeed = 0, y, x;
	while(simulationSpeed<1||simulationSpeed>100){
		mvprintw(maxy-1,(maxx/2)-18," Enter simulation speed (1-100):    ");
		getyx(stdscr,y,x);  // Get current cursor position.
		move(y,x-4);  // Move back cursor three steps.
		scanw("%d",&simulationSpeed);
	}
	drawOuterWall(); // Redraw outer wall.
	return simulationSpeed;
}


bool hitHandleBeing(Being *beingPrev, Being *beingToHitCheck, Attackposition *attackposition)
{
	//only compare if attackposition is set
	if(attackposition->posx != 0 && attackposition->posy != 0){

		// attack successful/failed
		if(getRndNum(2)!=2)
			return FALSE;

		// compare being position against attack
		if(beingToHitCheck->posx == attackposition->posx && beingToHitCheck->posy == attackposition->posy){
			attackposition->posx = 0;  // reset attack position
			attackposition->posy = 0;
			beingToHitCheck->hitpoints--;
			if(beingToHitCheck->hitpoints > 0){
				init_pair(COLOR_RED,COLOR_RED,-1);  // being turns red when taking damage
				attron(COLOR_PAIR(COLOR_RED));
				mvprintw(beingToHitCheck->posy,beingToHitCheck->posx,"*");
				attroff(COLOR_PAIR(COLOR_RED));
				beingToHitCheck->isHit = TRUE;
			}
			else{  // destroy being when out of hitpoints
				init_pair(COLOR_MAGENTA,COLOR_MAGENTA,-1);  // make being magenta
				attron(COLOR_PAIR(COLOR_MAGENTA));
				mvprintw(beingToHitCheck->posy,beingToHitCheck->posx,"*");
				attroff(COLOR_PAIR(COLOR_MAGENTA));

				// first being
				if(beingPrev==beingToHitCheck){
					// one being left
					if(beingToHitCheck->next==NULL)
						beingPrev = NULL;
					else
						beingPrev->next = beingToHitCheck->next;
					free(beingToHitCheck);
					return TRUE;
				}

				// last being
				if(beingToHitCheck->next==NULL){
					beingPrev->next = NULL;
					free(beingToHitCheck);
					return TRUE;
				}

				// any other being
				if(beingPrev!=NULL){
					beingPrev->next = beingToHitCheck->next;
					free(beingToHitCheck);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


void runWorld()
{
	unsigned int nbrOfGreenBeings, nbrOfBlueBeings;
	int i, simulationSpeed;
	Attackposition attackposition;
	MyColor greenBeingColor, blueBeingColor;
	gamemode = FREEROAM;
	attackposition.posy = 0;
	attackposition.posx = 0;
	bool beingDestroyed = FALSE;
	greenBeingColor = GREEN;
	nbrOfGreenBeings = getNbrOfBeings(&greenBeingColor);
	Being *greenBeingsHead = malloc(sizeof(Being));
	Being *greenBeingsCursor = malloc(sizeof(Being));
	Being *greenBeingsPrev = malloc(sizeof(Being));

	greenBeingsCursor = greenBeingsHead;

	for(i=0;i<nbrOfGreenBeings;i++){
		spawnBeing(greenBeingsCursor, &greenBeingColor);
		refresh();
		usleep(300);  // delay
	}
	blueBeingColor = BLUE;
	nbrOfBlueBeings = getNbrOfBeings(&blueBeingColor);
	Being *blueBeingsHead = malloc(sizeof(Being));
	Being *blueBeingsCursor = malloc(sizeof(Being));
	Being *blueBeingsPrev = malloc(sizeof(Being));

	blueBeingsCursor = blueBeingsHead;

	for(i=0;i<nbrOfBlueBeings;i++){
		spawnBeing(blueBeingsCursor, &blueBeingColor);
		refresh();
		usleep(300);  // delay
	}
	drawOuterWall();
	simulationSpeed = setSimulationSpeed();
	mvprintw(maxy-1,(maxx/2)-19," Press Enter to start simulation. ");
	getch();
	drawOuterWall();
	nodelay(stdscr, TRUE);  // Don't stop by getch().
	noecho();  // Don't type user input keys to screen during simulation.
	int ch = 0;
	while(ch != 27){
		ch=getch();
		// turn all beings
		greenBeingsCursor = greenBeingsHead;
		while(greenBeingsCursor->next!=NULL){
			turnBeing(greenBeingsCursor->next, &attackposition);
			greenBeingsCursor = greenBeingsCursor->next;
			blueBeingsCursor = blueBeingsHead;
			blueBeingsPrev = blueBeingsCursor;  // reset prev pointer
			beingDestroyed = FALSE;
			while(blueBeingsCursor->next!=NULL){  // compare all opponents against attack coordinates
				blueBeingsCursor = blueBeingsCursor->next;
				beingDestroyed = hitHandleBeing(blueBeingsPrev, blueBeingsCursor, &attackposition);
				if(beingDestroyed)
					break;
				blueBeingsPrev = blueBeingsCursor;
			}
			// reset attack coordinates after every failed attack
			attackposition.posy = 0;
			attackposition.posx = 0;
		}

		blueBeingsCursor = blueBeingsHead;
		while(blueBeingsCursor->next!=NULL){
			turnBeing(blueBeingsCursor->next, &attackposition);
			blueBeingsCursor = blueBeingsCursor->next;
			// handle attack hits on all enemy beings
			greenBeingsCursor = greenBeingsHead;
			greenBeingsPrev = greenBeingsCursor;  // reset prev pointer
			beingDestroyed = FALSE;
			while(greenBeingsCursor->next!=NULL){  // compare all opponents against attack coordinates
				greenBeingsCursor = greenBeingsCursor->next;
				beingDestroyed = hitHandleBeing(greenBeingsPrev, greenBeingsCursor, &attackposition);
				if(beingDestroyed)
					break;
				greenBeingsPrev = greenBeingsCursor;
				
			}
			// reset attack coordinates after every failed attack
			attackposition.posy = 0;
			attackposition.posx = 0;
		}
		refresh();

		// Change simulation speed or number of beings during run.
		if(ch=='+' && simulationSpeed<100)
			simulationSpeed++;
		else if(ch=='-' && simulationSpeed>1)
			simulationSpeed--;

		// Change gamemode
		else if(ch=='f' || ch=='F')
			gamemode = FREEROAM;
		else if(ch=='r' || ch=='R')
			gamemode = REGROUP;
		else if(ch=='a' || ch=='A')
			gamemode = ATTACK;

		usleep(1000000/simulationSpeed);
	}
	//free(greenBeingsHead);
	//free(greenBeingsCursor);
	//free(greenBeingsPrev);
	//free(blueBeingsHead);
	//free(blueBeingsCursor);
	//free(blueBeingsPrev);
	getch();
	endwin();
}


bool checkIfCoordinatesAreClear(const int *x, const int *y)  // check at init spawn
{
	bool isClear;
	char checksquare;
	checksquare = mvinch(*y,*x) & A_CHARTEXT;
	if(checksquare == ' ')
		isClear = TRUE;
	else
		isClear = FALSE;
	return isClear;
}
