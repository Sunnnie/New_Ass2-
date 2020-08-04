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
	int submittedLocs = 0;

	//ATTACKING HUNTERS
	//Finding Where the Hunters Can Go
	
	
	//int *numHunterPossibleLocs = 0;
	
	int *numDraculaPossibleLocs = 0; 
	/*
	for (int i=0; i<=4; i++) {
		*numHunterPossibleLocs = 0;
		PlaceId *HunterPossibleLocs = DvWhereCanTheyGo(dv, i, numHunterPossibleLocs);
		for (int j= 0; j < *numHunterPossibleLocs; j++) {
			*numDraculaPossibleLocs = 0; 
			PlaceId *DraculaPossibleLocs = DvWhereCanIGo(dv, numDraculaPossibleLocs);
			for (int k=0; k < *numDraculaPossibleLocs; k++) {
				if (DraculaPossibleLocs[k] == HunterPossibleLocs[j]) {
					if (*numDraculaPossibleLocs > k) {
						PlaceId suggestedLocation = DraculaPossibleLocs[k];
						submittedLocs++;
						registerBestPlay(placeIdToAbbrev(suggestedLocation), "Mwahahaha"); 
					}
				}
			}
		}
	}*/
	
	//Where Dracula Can Go
	/*
	int *numDraculaPossibleLocs = 0; 
	PlaceId *DraculaPossibleLocs = DvGetValidMoves(dv, numDraculaPossibleLocs);
	*/
	//AVOIDING THE SEA 
	/*
	*numHunterPossibleLocs = 0;
	*numDraculaPossibleLocs = 0; 
	for (int i=0; i<=4; i++) {
		*numHunterPossibleLocs = 0;
		PlaceId *HunterPossibleLocs = DvWhereCanTheyGo(dv, i, numHunterPossibleLocs);
		for (int j= 0; j < *numHunterPossibleLocs; j++) {
			*numDraculaPossibleLocs = 0; 
			PlaceId *DraculaPossibleLocs = DvWhereCanIGoByType(dv, true, false, numDraculaPossibleLocs);
			for (int k=0; k < *numDraculaPossibleLocs; k++) {
				if (DraculaPossibleLocs[k] != HunterPossibleLocs[j]) {
					if (*numDraculaPossibleLocs > k) {
						PlaceId suggestedLocation = DraculaPossibleLocs[k];
						registerBestPlay(placeIdToAbbrev(suggestedLocation), "Mwahahaha"); 
						submittedLocs++;
					}
				}
			}
		}
	}

	*/
	//PLACING VAMPIRES
	if (DvGetRound(dv) >= 10) {
		registerBestPlay(placeIdToAbbrev(HIDE), "Mwahahahaha");
		submittedLocs++;
	}

	
	//RUNNING AWAY FROM HUNTERS
	int round = DvGetRound(dv);
	if (round == 0) {
		registerBestPlay("CD", "Mwahahahaha");
		submittedLocs++;
	} else {
		//first priority: eliminate possible moves from dracula which is the same as hunters' next moves 
		int vaild_locs = 0; 
		PlaceId *D_locations = DvWhereCanIGo(dv, &vaild_locs); 
		PlaceId *possible_moves = malloc(sizeof(PlaceId)*NUM_REAL_PLACES); 

		PlaceId *hunters_moves = malloc(sizeof(PlaceId)*NUM_REAL_PLACES); 
		int num = 0; 
		int Lord_G = 0; 
		PlaceId *L_location = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &Lord_G);
		for (int i = 0; i < Lord_G; i++) {
			hunters_moves[num] = L_location[i];
			num++;
		}

		int Dr_S = 0;
		PlaceId *S_locations = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &Dr_S);
		for (int i = 0; i < Dr_S; i++) {
			hunters_moves[num] = S_locations[i];
			num++; 
		}

		int Van_H = 0;
		PlaceId *V_locations = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &Van_H);
		for (int i = 0; i < Van_H; i++) {
			hunters_moves[num] = V_locations[i];
			num++; 
		}

		int Mina_H = 0;
		PlaceId *M_locations = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, &Mina_H);
		for (int i = 0; i < Mina_H; i++) {
			hunters_moves[num] = M_locations[i];
			num++; 
		}

		int possible = 0;
		for (int i = 0; i < vaild_locs; i++) {
			int found = 0; 
			for (int j = 0; j < num; j++) {
				if (D_locations[i] == hunters_moves[j]) found = 1; 
			}
			if (found == 0) {
				possible_moves[possible] = D_locations[i];
				possible++; 
			}
		}

		if (possible == 0) {
			PlaceId ToGo = D_locations[0];
			registerBestPlay(placeIdToAbbrev(ToGo), "NoWhereToHide"); 
			submittedLocs++;
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
		if (*numDraculaPossibleLocs >= 1) {
			PlaceId suggestedLoc = DraculaPossibleLocs[0];
			registerBestPlay(placeIdToAbbrev(suggestedLoc), "Mwahahahaha"); 	
		} else {
			registerBestPlay(placeIdToAbbrev(HIDE), "Mwahahahaha"); 
		}
	}
}
