#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
//#include <sys/ioctl.h>
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
	initscr();
	curs_set(0);
	start_color();
	use_default_colors();
	srand(time(NULL));  // world will be generated randomly.
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


//void spawnBeings(Being *beingsCursor, const unsigned int *nbrOfBeings, const MyColor *beingColor)
//{
//	int i;
//	bool beingCreated;
//	for(i=0;i<*nbrOfBeings;i++){  // Spawn all beings.
//		usleep(300);  // delay
//		spawnBeing(&beings[i], &i, beingColor);
//		//if(i==0){  // Set special color on genesis being.
//		//	beings[0].myColor=15;
//		//	turnBeing(&beings[i], &i);
//		//}
//		refresh();
//	}
//}


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


void hitHandleBeing(Being *beingPrev, Being *beingToHitcheckHead, Being *beingToHitCheckCursor, Attackposition *attackposition)
{

	//if(!beingToHitcheckCursor->alive)  // do nothing if being is not alive
	//	return;

	//only compare if attackposition is set
	if(attackposition->posx != 0 && attackposition->posy != 0){

		// attack successful/failed
		if(getRndNum(2)!=2)
			return;

		// compare being position against attack
		if(beingToHitcheck->posx == attackposition->posx && beingToHitcheck->posy == attackposition->posy){
			attackposition->posx = 0;  // reset attack position
			attackposition->posy = 0;
			beingToHitcheck->hitpoints--;
			if(beingToHitcheck->hitpoints > 0){
				init_pair(COLOR_RED,COLOR_RED,-1);  // being turns red when taking damage
				attron(COLOR_PAIR(COLOR_RED));
				mvprintw(beingToHitcheck->posy,beingToHitcheck->posx,"*");
				attroff(COLOR_PAIR(COLOR_RED));
				beingToHitcheck->isHit = TRUE;
			}
			else{  // destroy being when out of hitpoints
				init_pair(COLOR_MAGENTA,COLOR_MAGENTA,-1);  // make being magenta
				attron(COLOR_PAIR(COLOR_MAGENTA));
				mvprintw(beingToHitcheck->posy,beingToHitcheck->posx,"*");
				attroff(COLOR_PAIR(COLOR_MAGENTA));
				//beingToHitcheck->alive=FALSE;  // not alive when hitpoints reach zero
				
				// first being
				if(beingToHitCheckCursor==beingToHitcheckHead){
					beingToHitcheckHead = NULL;
					free(beingToHitCheckCursor);
					return;
				}

				// last being
				if(beingToHitCheckCursor->next==NULL){
					beingPrev->next = NULL;
					free(beingToHitCheckCursor);
					return;
				}

				// any other being
				if(beingPrev!=NULL){
					//Being *temp = beingToHitCheckCursor->next;
					beingPrev->next = beingToHitCheckCursor->next;
					free(beingToHitCheckCursor);
					return;
				}
			}
		}
	}
}


void runWorld()
{
	unsigned int nbrOfGreenBeings, nbrOfBlueBeings;
	int i, j, simulationSpeed;
	Attackposition attackposition;
	// int newBeingToSpawnNbr;  ////////////////////////////////////////////////////////////////  used in create/remove
	//bool beingCreated;    ////////////////////////////////////////////////////////////////  used in create/remove
	MyColor greenBeingColor, blueBeingColor;
	gamemode = FREEROAM;
	attackposition.posy = 0;
	attackposition.posx = 0;
	greenBeingColor = GREEN;
	nbrOfGreenBeings = getNbrOfBeings(&greenBeingColor);
	Being *greenBeingsHead;
	Being *greenBeingsCursor = greenBeingsHead;  // = malloc(nbrOfGreenBeings*sizeof(Being));
	Being *greenBeingsPrev;
	//spawnBeings(&*greenBeingsCursor,&nbrOfGreenBeings,&greenBeingColor);
	blueBeingColor = BLUE;
	nbrOfBlueBeings = getNbrOfBeings(&blueBeingColor);
	Being *blueBeingsHead;
	Being *blueBeingsCursor = blueBeingsHead;  // = malloc(nbrOfGreenBeings*sizeof(Being));
	Being *blueBeingsPrev;
	//spawnBeings(&*blueBeingsCursor,&nbrOfBlueBeings,&blueBeingColor);
	for(i=0;i<nbrOfBlueBeings;i++)
		spawnBeing(&*blueBeingsCursor, &blueBeingColor)
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
		*greenBeingsCursor = greenBeingsHead;
		*blueBeingsCursor = blueBeingsHead;
		// turn all beings
		while(greenBeingsCursor->next!=NULL){
			turnBeing(greenBeingsCursor, &attackposition);
			greenBeingsCursor = greenBeingsCursor->next;
			// handle attack hits on all enemy beings
			//for(j=0;j<nbrOfBlueBeings;j++)
			blueBeingsPrev = blueBeingsCursor;  // reset prev pointer
			while(blueBeingsCursor->next!=NULL){
				hitHandleBeing(blueBeingsPrev, blueBeingsHead, blueBeingsCursor, &attackposition);
				blueBeingsPrev = blueBeingsCursor;
				blueBeingsCursor = blueBeingsCursor->next;
			}
		}
		*greenBeingsCursor = greenBeingsHead;
		*blueBeingsCursor = blueBeingsHead;
		//for(i=0;i<nbrOfBlueBeings;i++){
		while(blueBeingsCursor->next!=NULL){
			turnBeing(blueBeingsCursor, &attackposition);
			blueBeingsCursor = blueBeingsCursor->next;
			// handle attack hits on all enemy beings
			//for(j=0;j<nbrOfGreenBeings;j++)
			greenBeingsPrev = greenBeingsCursor;  // reset prev pointer
			while(greenBeingsCursor->next!=NULL){
				hitHandleBeing(greenBeingsPrev, greenBeingsHead, greenBeingsCursor, &attackposition);
				greenBeingsPrev = greenBeingsCursor;
				greenBeingsCursor = greenBeingsCursor->next;
			}
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




		// Create and remove beings during run.
		//if(ch==',' && nbrOfBeings > 1){
		//	mvprintw(beings[nbrOfBeings-1].posy,beings[nbrOfBeings-1].posx," ");  // Erase last being.
		//	nbrOfBeings--;
		//	beings = realloc(beings,nbrOfBeings*sizeof(Being));
		//}
		//else if(ch=='.' && nbrOfBeings < 65535){  // Create new being.
		//	nbrOfBeings++;
		//	beings = realloc(beings,nbrOfBeings*sizeof(Being));
		//	newBeingToSpawnNbr = nbrOfBeings - 1;
		//	beingCreated = spawnBeing(&beings[newBeingToSpawnNbr], &newBeingToSpawnNbr);
		//	if(beingCreated==FALSE){ // If being created is unsuccessful due to out of space.
		//		nbrOfBeings--;
		//		beings = realloc(beings,nbrOfBeings*sizeof(Being));
		//	}
		//}
		usleep(1000000/simulationSpeed);
	}
	free(greenBeings);
	free(blueBeings);
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
