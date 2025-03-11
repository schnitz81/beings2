#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "world.h"
#include "event.h"
#include "being.h"

void look_ahead(Being *beingToTurn)
{
	// Handle revolution crossover.
	switch(beingToTurn->myHeading){
		case OVERVALUE1:
			beingToTurn->myHeading = UP;
			break;
		case OVERVALUE2:
			beingToTurn->myHeading = UPRIGHT;
			break;
		case UNDERVALUE1:
			beingToTurn->myHeading = UPLEFT;
			break;
		case UNDERVALUE2:
			beingToTurn->myHeading = LEFT;
			break;
		default:
			break;
	}

	Object surrounding[5][5];
	char square;
	int i,j;
	for(j=0;j<=4;j++){	// Update obstacles of the current surroundings.
		for(i=0;i<=4;i++){
			square = mvinch((beingToTurn->posy-2)+j,(beingToTurn->posx-2)+i) & A_CHARTEXT;  // Get square content.
			if(square==' ')
				surrounding[i][j] = NONE;
			else if(square=='*')
				surrounding[i][j] = OTHERBEING;
			else if(square=='=' || square=='|' || square=='#')
				surrounding[i][j] = FENCE;
		}
	}

	/* Being field of view:
	#####
	#####	
	##X##
	#####
	##### 
	*/

	// Being peeks at surrounding ahead depending on heading.
	switch(beingToTurn->myHeading){
		case UP:
			beingToTurn->obstacles.leftfar = surrounding[1][0];
			beingToTurn->obstacles.leftnear = surrounding[1][1];
			beingToTurn->obstacles.middlefar = surrounding[2][0];
			beingToTurn->obstacles.middlenear = surrounding[2][1];
			beingToTurn->obstacles.rightfar = surrounding[3][0];
			beingToTurn->obstacles.rightnear = surrounding[3][1];
			break;
		case UPRIGHT:
			beingToTurn->obstacles.leftfar = surrounding[3][0];
			beingToTurn->obstacles.leftnear = surrounding[2][1];
			beingToTurn->obstacles.middlefar = surrounding[4][0];
			beingToTurn->obstacles.middlenear = surrounding[3][1];
			beingToTurn->obstacles.rightfar = surrounding[4][1];
			beingToTurn->obstacles.rightnear = surrounding[3][2];
			break;
		case RIGHT:
			beingToTurn->obstacles.leftfar = surrounding[4][1];
			beingToTurn->obstacles.leftnear = surrounding[3][1];
			beingToTurn->obstacles.middlefar = surrounding[4][2];
			beingToTurn->obstacles.middlenear = surrounding[3][2];
			beingToTurn->obstacles.rightfar = surrounding[4][3];
			beingToTurn->obstacles.rightnear = surrounding[3][3];
			break;
		case DOWNRIGHT:
			beingToTurn->obstacles.leftfar = surrounding[4][3];
			beingToTurn->obstacles.leftnear = surrounding[3][2];
			beingToTurn->obstacles.middlefar = surrounding[4][4];
			beingToTurn->obstacles.middlenear = surrounding[3][3];
			beingToTurn->obstacles.rightfar = surrounding[3][4];
			beingToTurn->obstacles.rightnear = surrounding[2][3];
			break;
		case DOWN:
			beingToTurn->obstacles.leftfar = surrounding[3][4];
			beingToTurn->obstacles.leftnear = surrounding[3][3];
			beingToTurn->obstacles.middlefar = surrounding[2][4];
			beingToTurn->obstacles.middlenear = surrounding[2][3];
			beingToTurn->obstacles.rightfar = surrounding[1][4];
			beingToTurn->obstacles.rightnear = surrounding[1][3];
			break;
		case DOWNLEFT:
			beingToTurn->obstacles.leftfar = surrounding[1][4];
			beingToTurn->obstacles.leftnear = surrounding[2][3];
			beingToTurn->obstacles.middlefar = surrounding[0][4];
			beingToTurn->obstacles.middlenear = surrounding[1][3];
			beingToTurn->obstacles.rightfar = surrounding[0][3];
			beingToTurn->obstacles.rightnear = surrounding[1][2];
			break;
		case LEFT:
			beingToTurn->obstacles.leftfar = surrounding[0][3];
			beingToTurn->obstacles.leftnear = surrounding[1][3];
			beingToTurn->obstacles.middlefar = surrounding[0][2];
			beingToTurn->obstacles.middlenear = surrounding[1][2];
			beingToTurn->obstacles.rightfar = surrounding[0][1];
			beingToTurn->obstacles.rightnear = surrounding[1][1];
			break;
		case UPLEFT:
			beingToTurn->obstacles.leftfar = surrounding[0][1];
			beingToTurn->obstacles.leftnear = surrounding[1][2];
			beingToTurn->obstacles.middlefar = surrounding[0][0];
			beingToTurn->obstacles.middlenear = surrounding[1][1];
			beingToTurn->obstacles.rightfar = surrounding[1][0];
			beingToTurn->obstacles.rightnear = surrounding[2][1];
			break;
		default:
			break;
	}	
}

void decision(Being *beingToTurn)
{
	int i;
	
	// If movement is stopped, decide start moving or not. *EARLY*
	if(beingToTurn->resting){
		if(getRndNum(2)==2){
			beingToTurn->myHeading = getRndNum(8)-1;
			beingToTurn->resting = FALSE;	
		}
		else 	
			return; // No other activity this round if decided to stand still.
	}	
	
	// Look towards path ahead for obstacles.
	look_ahead(beingToTurn);
	
	bool firstCheck = TRUE;
	
	// Keep deciding until the coast is clear.
	while(firstCheck || (beingToTurn->obstacles.middlenear!=NONE && !beingToTurn->resting)){
		
		i = getRndNum(16);
		
		// Try with an entirely new heading by 50% chance if first choice was blocked.
		if(!firstCheck){ 
			if(getRndNum(2)==2){
				beingToTurn->myHeading = getRndNum(8)-1;
				beingToTurn->resting = FALSE;
			}
		}
		
		// Change behaviour by own will (only one notch).
		// Turn left by own will.
		if(i==5)
			beingToTurn->myHeading--;
		// Turn right by own will.
		else if(i==6)
			beingToTurn->myHeading++;
		// Stop by own will.
		else if(i==16)
			beingToTurn->resting=TRUE;
		
		// Look towards path ahead for obstacles.
		look_ahead(beingToTurn);
		
		// Handling of other beings *****************************************************************************
		
		// If another being is far away.
		if(beingToTurn->obstacles.leftnear==NONE && beingToTurn->obstacles.middlenear==NONE && beingToTurn->obstacles.rightnear==NONE){
			// being obstacle far left.
			if(beingToTurn->obstacles.leftfar==OTHERBEING && beingToTurn->obstacles.middlefar==NONE && beingToTurn->obstacles.rightfar==NONE)
				beingToTurn->myHeading++;
			// being obstacle far right
			else if(beingToTurn->obstacles.leftfar==NONE && beingToTurn->obstacles.middlefar==NONE && beingToTurn->obstacles.rightfar==OTHERBEING)
				beingToTurn->myHeading--;
			// being obstacle far middle
			else if(beingToTurn->obstacles.leftfar==NONE && beingToTurn->obstacles.middlefar==OTHERBEING && beingToTurn->obstacles.rightfar==NONE){
				if(getRndNum(2)==1)
					beingToTurn->myHeading--;
				else
					beingToTurn->myHeading++;
			}
		}
	
		// Likely stop if another being is close ahead.
		if(beingToTurn->obstacles.leftnear==OTHERBEING || beingToTurn->obstacles.middlenear==OTHERBEING || beingToTurn->obstacles.rightnear==OTHERBEING){
			if(getRndNum(4)!=4)
				beingToTurn->resting = TRUE;
		}
			
		
		// ********************************************************************************************************
	
		// Look towards path ahead for obstacles.
		look_ahead(beingToTurn);
	
		// Special handling of fences (outside the standard collision avoidance) **********************************
	
		// If fence spotted straight ahead (in the middle) and nothing is closer: 
		if(beingToTurn->obstacles.leftfar==NONE && beingToTurn->obstacles.middlenear==NONE && beingToTurn->obstacles.rightnear==NONE){
			if(beingToTurn->obstacles.middlefar==FENCE){
				if(beingToTurn->obstacles.rightfar==FENCE && beingToTurn->obstacles.leftfar==NONE)
					beingToTurn->myHeading--;
				else if(beingToTurn->obstacles.rightfar==NONE && beingToTurn->obstacles.leftfar==FENCE)	
					beingToTurn->myHeading++;
				else if(beingToTurn->obstacles.rightfar==NONE && beingToTurn->obstacles.leftfar==NONE){	
					if(getRndNum(2)==1)
						beingToTurn->myHeading--;
					else
						beingToTurn->myHeading++;
				}
			}
			
		}
		
		// Look towards path ahead for obstacles.
		look_ahead(beingToTurn);
		
		// Increase chance of stopping if facing a fence.
		if(beingToTurn->obstacles.middlenear==FENCE && getRndNum(6)>4)
			beingToTurn->resting = TRUE;
			
		// ************************************************************************************************************
	
		// Look towards path ahead for obstacles.
		look_ahead(beingToTurn);
		firstCheck = FALSE;
	}  // while coast isn't clear.
}

