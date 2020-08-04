////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"

void decideDraculaMove(DraculaView dv)
{
	/*
	//Finding Where Dracula can Go
	int *numDraculaPossibleLocs = 0; 
	PlaceId *DraculaPossibleLocs = DvGetValidMoves(dv, *numDraculaPossibleLocs);

	
	
	//Executing the Move
	*/
	int submittedLocs = 0;

	//ATTACKING HUNTERS
	//Finding Where the Hunters Can Go
	int *numHunterPossibleLocs = 0;
	int *numDraculaPossibleLocs = 0; 
	for (int i=0; i<=4; i++) {
		*numHunterPossibleLocs = 0;
		PlaceId *HunterPossibleLocs = DvWhereCanTheyGo(dv, i, numHunterPossibleLocs);
		for (int j= 0; j < *numHunterPossibleLocs; j++) {
			*numDraculaPossibleLocs = 0; 
			PlaceId *DraculaPossibleLocs = DvWhereCanIGo(dv, numDraculaPossibleLocs);
			for (int k=0; k < *numDraculaPossibleLocs; k++) {
				if (DraculaPossibleLocs[k] == HunterPossibleLocs[j]) {
					PlaceId suggestedLocation = DraculaPossibleLocs[k];
					submittedLocs++;
					registerBestPlay(placeIdToAbbrev(suggestedLocation), "Mwahahaha"); 
				}
			}
		}
	}

	//Where Dracula Can Go
	/*
	int *numDraculaPossibleLocs = 0; 
	PlaceId *DraculaPossibleLocs = DvGetValidMoves(dv, numDraculaPossibleLocs);
	*/
	//AVOIDING THE SEA

	//PLACING VAMPIRES

	//RUNNING AWAY FROM HUNTERS
	*numHunterPossibleLocs = 0;
	*numDraculaPossibleLocs = 0; 
	for (int i=0; i<=4; i++) {
		*numHunterPossibleLocs = 0;
		PlaceId *HunterPossibleLocs = DvWhereCanTheyGo(dv, i, numHunterPossibleLocs);
		for (int j= 0; j < *numHunterPossibleLocs; j++) {
			*numDraculaPossibleLocs = 0; 
			PlaceId *DraculaPossibleLocs = DvWhereCanIGo(dv, numDraculaPossibleLocs);
			for (int k=0; k < *numDraculaPossibleLocs; k++) {
				if (DraculaPossibleLocs[k] != HunterPossibleLocs[j]) {
					PlaceId suggestedLocation = DraculaPossibleLocs[k];
					registerBestPlay(placeIdToAbbrev(suggestedLocation), "Mwahahaha"); 
					submittedLocs++;
				}
			}
		}
	}
	//RECOVER HEALTH AT CASTLE DRACULA
	if (DvGetHealth(dv, PLAYER_DRACULA) <= 20) {
		registerBestPlay("CD", "Mwahahahaha");
		submittedLocs++;
	}

	//PROTECTION FEATURE (IF NO SUBMITTED LOCATIONS)
	if (submittedLocs == 0) {
		*numDraculaPossibleLocs = 0; 
		PlaceId *DraculaPossibleLocs = DvWhereCanIGo(dv, numDraculaPossibleLocs);
		if (*numDraculaPossibleLocs != 0) {
			PlaceId suggestedLoc = DraculaPossibleLocs[0];
			registerBestPlay(placeIdToAbbrev(suggestedLoc), "Mwahahahaha"); 	
		} else {
			registerBestPlay(placeIdToAbbrev(HIDE), "Mwahahahaha"); 
		}
	}
}
