////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdio.h>

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

PlaceId startLoc(HunterView hv, Player hunter);

///////////////////////////////////////////////////////////////////////
// STRATEGY IDEAS
//
//  Goals:
//		a) Kill dracula (end game)
//		b) Kill immature vamps (points)
//		c) Prevent dracula from doing what it wants to do
//		d) Not die (rest)
//
//  Achieving Goals
// 	(a)
//		- Do research somewhat regularly to get idea of where drac is
//		- Get all possible drac locations given last known location in round X
//		- Move towards best possible locations for drac via shortest path
//      - REPEAT above
//			- Likely we will bump into trail showing a newer prev drac location narrowing down possible locs
//
//	(b)
//		- If location of vamp known get shortest path to vamp
//      - (Most likely) vamp on map but location unkown
//			- Find drac trail and try to find vamp in it
//
// 	(c)
//		What dracula wants to do (drac stategy)
//			- Wants to get to castle drac if hunters not nearby
//				- [Good strat] lure hunters away from CD then surround self with tail
//					and teleport to CD
//			- Wants to go locs with high degree (easier to escape)
//			- Avoid hunters


void decideHunterMove(HunterView hv)
{

	PlaceId move = NOWHERE;
	Message msg;
	Round round = HvGetRound(hv);
	Player hunter = HvGetPlayer(hv);
	int health = HvGetHealth(hv, hunter);
	PlaceId curPlace = HvGetPlayerLocation(hv, hunter);
	int lastDracLocRound = 0;
	PlaceId lastDracLoc = HvGetLastKnownDraculaLocation(hv, &lastDracLocRound);

	// Calc immature vampire stats
	PlaceId vampLoc = HvGetVampireLocation(hv);
	int roundsToMature = roundsTillVampMatures(round, vampLoc);
	int pathToVampSize = 0;
	PlaceId *pathToVamp = HvGetShortestPathTo(hv, hunter, vampLoc, &pathToVampSize);
	bool canKillVamp = vampLoc != NOWHERE && pathToVampSize <= roundsToMature;
	//printf("%s %d %d\n", placeIdToName(vampLoc), roundsToMature, pathToVampSize);

	if (round == 0) {
		sprintf(msg, "Picking start location");
		move = startLoc(hv, hunter);
	} else if (health < 4) {
		sprintf(msg, "Resting hunter");
		move = curPlace;
	} else if (canKillVamp) {
		sprintf(msg, "Killing vamp known loc and can reach in time");
		move = pathToVamp[0];
	} else if (lastDracLoc != NOWHERE) {
		sprintf(msg, "Hunting Drac: Last known = %s, Round = %d", placeIdToAbbrev(lastDracLoc), lastDracLocRound);
		// TODO set move
	} else {
		sprintf(msg, "Default");
		move = curPlace;
	}

	// Confirm move is legal and if not replace move and add error msg
	bool invalidStart = round == 0 && move == NOWHERE;
	int numValidLocs = 0;
	PlaceId *validLocs = HvWhereCanIGo(hv, &numValidLocs);
	bool invalidMove = round != 0 && containsPlace(validLocs, numValidLocs, move) < 0;
	if (invalidStart || invalidMove) {
		sprintf(msg + strlen(msg), " | Invalid move: %s, replaced with valid", placeIdToName(move));
		if (round == 0) {
			move = CASTLE_DRACULA;
		} else {
			move = validLocs[0];
		}
	}
	
	registerBestPlay(placeIdToAbbrev(move), msg);
}

PlaceId startLoc(HunterView hv, Player hunter) {
	if (hunter == PLAYER_LORD_GODALMING) {
		return CASTLE_DRACULA;
	} else if (hunter == PLAYER_DR_SEWARD) {
		// Within one move of CD
		return CASTLE_DRACULA;
	} else if (hunter == PLAYER_VAN_HELSING) {
		// Within two moves away from CD
		return CASTLE_DRACULA;
	} else {
		// Place in highly connected city
		return CASTLE_DRACULA;
	}
}