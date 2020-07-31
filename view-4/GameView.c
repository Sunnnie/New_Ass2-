////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"



// add your own #includes here
#define MAX_TRAPS 3 
#define MOVES_SIZE 7
#define ROUND_SIZE (MOVES_SIZE * NUM_PLAYERS + NUM_PLAYERS) // size of each round
#define LAST_ROUND_SIZE (ROUND_SIZE-1) // size of last round
#define MOVE_DELIM " "
#define PLACE_ABBREV_SIZE 2
#define START_PLAY_ACTION 3
#define MAX_TOTAL_TRAPS 6

// TODO: ADD YOUR OWN STRUCTS HERE

struct playerStats{
	int health;
	PlaceId *moveHist; //chronological order
	int moveHistSize;//stores size of array
	PlaceId *locHist; // chrono order
	int locHistSize;//stores size of array 
};
typedef struct playerStats * PlayerStats;

struct gameView {
	int gamescore;
	Round round;
	PlaceId *trapLocations; // array of trap locations
	int trapLocationSize; 
	PlaceId vampLocation;
	PlayerStats *playerStats; //array of playerstats
	Player player; // current player
	PlaceId *trail; // queue array of trail locations
	int trailSize;
	Map map;
};

void handleHunter(GameView gv, char *move, Player player);
void handleDracula(GameView gv, char *move);
void handlePastPlays(char *pastPlays, GameView gv);
void updateHealth(GameView gv, int healthChange, Player player);
void dfsReachable(GameView gv, Queue q, PlaceId p, bool road, bool rail, bool boat, 
				int *seen, int dist, int maxDist, bool isDrac);
int maxRailDist (Player player, Round round);
PlaceId getLocationFromMove(GameView gv, PlaceId move, int trailLocation);
////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	GameView new = malloc(sizeof(*new));
	assert(new != NULL);
	new->map = MapNew();
	new->gamescore = GAME_START_SCORE; // set gamescore 
	new->vampLocation = NOWHERE; // set intial vampire location
	new->trapLocations = malloc(MAX_TOTAL_TRAPS*sizeof(PlaceId));
	assert(new->trapLocations != NULL);
	new->trapLocationSize = 0;

	//initialise trap locations
	for (int i = 0; i < MAX_TOTAL_TRAPS; i++)
	{
		new->trapLocations[i] = NOWHERE; 
	}
	
	new->trail = malloc(TRAIL_SIZE *sizeof(PlaceId));
	new->trailSize = 0;

	//initialise trail
	for (int i = 0; i < TRAIL_SIZE; i++)
	{
		new->trail[i] = NOWHERE; 
	}
	//size of pastplays
	int pastPlaysSize = strlen(pastPlays); 
	//computes current round
	new->round = (pastPlaysSize + 1)/ROUND_SIZE; 
	//size of last round 
	int sizeofLastRound = pastPlaysSize % ROUND_SIZE; 

	//if first round or start of last round set to first player
	if (sizeofLastRound == 0 || sizeofLastRound == LAST_ROUND_SIZE) 
	{
		new->player = PLAYER_LORD_GODALMING;
	}else
	{
		new->player = sizeofLastRound/MOVES_SIZE; 
	}

	// dinamically allocate memory for playerstats array
	new->playerStats = malloc(NUM_PLAYERS*sizeof(PlayerStats)); 
	assert(new->playerStats != NULL);

	// loop initialises all player stats
	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		//allocates memory for playerstats structs
		new->playerStats[i] = malloc(sizeof(struct playerStats));
		assert(new->playerStats[i] != NULL);
		if (i == PLAYER_DRACULA)
		{
			new->playerStats[i]->health = GAME_START_BLOOD_POINTS;
		}else 
		{
			new->playerStats[i]->health = GAME_START_HUNTER_LIFE_POINTS;
		}

		//dynamically allocates memory to round move history
		new->playerStats[i]->moveHist = malloc((new->round + 1) * sizeof(PlaceId));
		assert(new->playerStats[i]->moveHist != NULL);
		
		//dynamically allocates memory to round loc history
		new->playerStats[i]->locHist = malloc((new->round + 1) * sizeof(PlaceId));
		assert(new->playerStats[i]->locHist != NULL);
		for (int j =0 ; j < new->round; j++)
		{
			new->playerStats[i]->moveHist[j] = NOWHERE;
			new->playerStats[i]->locHist[j] = NOWHERE;
		}
		new->playerStats[i]->moveHistSize = 0;
		new->playerStats[i]->locHistSize = 0;
	}
	handlePastPlays(pastPlays, new);

	return new;
}

void GvFree(GameView gv)
{
	free(gv->trapLocations);
	free(gv->trail);
	for (int i = 0; i < NUM_PLAYERS; i++)
	{
		free(gv->playerStats[i]->moveHist);
		free(gv->playerStats[i]->locHist);
		free(gv->playerStats[i]);
	}
	free(gv->playerStats);
	MapFree(gv->map);
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	return gv->player;
}

int GvGetScore(GameView gv)
{
	return gv->gamescore;
}

int GvGetHealth(GameView gv, Player player)
{
	return gv->playerStats[player]->health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	int last = gv->playerStats[player]->moveHistSize -1;
	if (last == -1)
		return NOWHERE;

	if (gv->playerStats[player]->health == 0)
		return HOSPITAL_PLACE;
	
	return gv->playerStats[player]->locHist[last];
}

PlaceId GvGetVampireLocation(GameView gv)
{	
	return gv->vampLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	*numTraps = gv->trapLocationSize;
	return copyPlaceArray(gv->trapLocations, gv->trapLocationSize);
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	return GvGetLastMoves(gv, player, gv->playerStats[player]->moveHistSize, numReturnedMoves, canFree);
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	assert(numMoves > 0);
	if (numMoves > gv->playerStats[player]->moveHistSize)
		numMoves = gv->playerStats[player]->moveHistSize;

	*numReturnedMoves = numMoves;
	*canFree = false;
	return gv->playerStats[player]->moveHist;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	return GvGetLastLocations(gv, player, gv->playerStats[player]->moveHistSize, numReturnedLocs, canFree);
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	assert(numLocs > 0);
	if (numLocs > gv->playerStats[player]->locHistSize)
		numLocs = gv->playerStats[player]->locHistSize;

	*numReturnedLocs = numLocs;
	*canFree = false;
	return gv->playerStats[player]->locHist;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{

	return GvGetReachableByType(gv, player, round, from, true, true, true, numReturnedLocs);
}


PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	Queue q = newQueue();
	QueueJoin(q, from);
	int dist = 0;
	int maxDist = maxRailDist(player, round);
	if (maxDist == 0)
		maxDist = 1;
	bool isDrac = player == PLAYER_DRACULA;
	int *seen = calloc(MapNumPlaces(gv->map), sizeof(int));
	assert(seen != NULL);
	dfsReachable(gv, q, from, road, rail, boat, 
				seen, dist, maxDist, isDrac);
	free(seen);
	int qsize = QueueSize(q);
	PlaceId *reachable = malloc(qsize *sizeof(PlaceId));
	assert(reachable != NULL);
	for (int i = 0; i < qsize; i++)
	{
		reachable[i] = QueueLeave(q);
	}
	*numReturnedLocs = qsize;
	dropQueue(q);
	return reachable;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

PlaceId *copyPlaceArray(PlaceId *array, int size)
{
	PlaceId *new = malloc(size * sizeof(PlaceId));
	assert(new != NULL);
	for (int i = 0; i < size; i++)
		new[i] = array[i];
	return new;
}

// Gets place ID from move
PlaceId getPlaceFromMove(char *move)
{
	char abbrev[PLACE_ABBREV_SIZE+1] = {0};//accounts for null string operator
	strncpy(abbrev, move+1, PLACE_ABBREV_SIZE);//skips first element and copies location into abbrev
	return placeAbbrevToId(abbrev); // see use of Function in Places.h
}

//Returns player name from the first element of move string 
Player playerFromChar(char c)
{
	if (c == 'G')
		return PLAYER_LORD_GODALMING;
	if (c == 'S')
		return PLAYER_DR_SEWARD;
	if (c == 'H')
		return PLAYER_VAN_HELSING;
	if (c == 'M')
		return PLAYER_MINA_HARKER;
	if (c == 'D')
		return PLAYER_DRACULA;
	return -1;
}

// TODO Updates health of Dracula and Hunters
void updateHealth(GameView gv, int healthChange, Player player)
{
	// update player health
	gv->playerStats[player]->health += healthChange; 
	if (player != PLAYER_DRACULA && gv->playerStats[player]->health > GAME_START_HUNTER_LIFE_POINTS)
		// set hunter health to max
		gv->playerStats[player]->health = GAME_START_HUNTER_LIFE_POINTS;  
		//if hunter health is negative or 0 set to 0
	if (gv->playerStats[player]->health <= 0)
	{
		gv->playerStats[player]->health = 0;
		// update game score -4
		if (player != PLAYER_DRACULA)
			gv->gamescore -= SCORE_LOSS_HUNTER_HOSPITAL; 
	}	
} 

// assuming the array is big enough to fit what we are inserting
void addPlace(PlaceId *places, int *size, PlaceId place)
{
	places[*size] = place;
	*size += 1;
}

// removes first place in an array that matches parameter "place" and shifts down to fill empty spots
void removePlace(PlaceId *places, int *size, PlaceId place)
{
	if (*size == 0)
		return;

	int pos = -1;
	for (int i = 0; i < *size; i++)
	{
		if (places[i] == place)
		{
			pos = i;
			break;
		}
	}
	//could not find place in array
	if (pos == -1)
		return;
	for (int i = pos; i < *size -1; i++)
	{
		places[i] = places[i+1];
	}
	places[*size-1] = NOWHERE;
	*size -= 1;
}

//handles the encounters for the hunters
void handleHunter(GameView gv, char *move, Player player)
{
	//gets placeId for the player
	PlaceId place = getPlaceFromMove(move); 
	int lastPos = gv->playerStats[player]->moveHistSize-1;

	if (lastPos >= 0 && gv->playerStats[player]->moveHist[lastPos] == place)
	{
		updateHealth(gv, LIFE_GAIN_REST, player);
	}

	addPlace(gv->playerStats[player]->moveHist, &(gv->playerStats[player]->moveHistSize), place);
	addPlace(gv->playerStats[player]->locHist, &(gv->playerStats[player]->locHistSize), place);

	// starts loop from action move
	for (int i = START_PLAY_ACTION; i < MOVES_SIZE; i++) 
	{
		char encounter = move[i];
		if (encounter == 'T')
		{
			updateHealth(gv, -LIFE_LOSS_TRAP_ENCOUNTER, player);
			removePlace(gv->trapLocations, &(gv->trapLocationSize), place);
			//TODO: confirm if hunter dies from trap check if vampire still alive, if vampire on same place is trap.
		}
		else if (encounter == 'V')
		{
			gv->vampLocation = NOWHERE;
		}
		else if (encounter == 'D')
		{
			updateHealth(gv, -LIFE_LOSS_DRACULA_ENCOUNTER, player);
			updateHealth(gv, -LIFE_LOSS_HUNTER_ENCOUNTER, PLAYER_DRACULA);
		}

	}
}

//handles the encounters for Dracula
void handleDracula(GameView gv, char *move)
{
	PlaceId movePlace = getPlaceFromMove(move);
	// place = loc
	PlaceId place = getLocationFromMove(gv, movePlace,gv->trailSize);
	Player player = PLAYER_DRACULA;

	addPlace(gv->playerStats[player]->moveHist, &(gv->playerStats[player]->moveHistSize), movePlace);
	addPlace(gv->playerStats[player]->locHist, &(gv->playerStats[player]->locHistSize), place);

	if (gv->trailSize == TRAIL_SIZE)
	{
		removePlace(gv->trail, &(gv->trailSize), gv->trail[0]);
	}
	addPlace(gv->trail, &(gv->trailSize), place);

	if (placeIsSea(place))
	{
		updateHealth(gv, -LIFE_LOSS_SEA, player);
	}

	if (place == CASTLE_DRACULA)
	{
		updateHealth(gv, LIFE_GAIN_CASTLE_DRACULA, player);
	}

	for (int i = START_PLAY_ACTION; i < MOVES_SIZE; i++)
	{
		char encounter = move[i];
		if (encounter == 'T')
		{
			if (gv->trapLocationSize == MAX_TOTAL_TRAPS)
				removePlace(gv->trapLocations, &(gv->trapLocationSize), gv->trapLocations[0]);
			addPlace(gv->trapLocations, &(gv->trapLocationSize), place);
		}
		// TO DO replace 4 with # define
		else if (encounter == 'V' && i == 4)
		{
			gv->vampLocation = place;
		}
		else if (encounter == 'M')
		{
			removePlace(gv->trapLocations, &(gv->trapLocationSize), place);
		}
		else if (encounter == 'V')
		{
			gv->vampLocation = NOWHERE;
			gv->gamescore -= SCORE_LOSS_VAMPIRE_MATURES;
		}
	}
	gv->gamescore -= SCORE_LOSS_DRACULA_TURN;
}

// handles all plays 
void handlePastPlays(char *pastPlays, GameView gv)
{
	char *pastPlayCopy = strdup(pastPlays);
	// splits pastplays into player moves
	char *move = strtok(pastPlayCopy, MOVE_DELIM); 
	while (move != NULL)
	{
		//process move
		Player player = playerFromChar(move[0]);
		if (GvGetHealth(gv, player) == 0 && player != PLAYER_DRACULA)
			gv->playerStats[player]->health = GAME_START_HUNTER_LIFE_POINTS;
		if (player != PLAYER_DRACULA)
		{
			handleHunter(gv, move, player);
		}else 
		{
			handleDracula(gv, move);
		}
		move = strtok(NULL, MOVE_DELIM); 
	}
	free(pastPlayCopy);
}

//gets location from move 
PlaceId getLocationFromMove(GameView gv, PlaceId move, int trailLocation)
{
	if (move == TELEPORT)
	{
		move = CASTLE_DRACULA;
	}
	else if (move == HIDE)
	{
		trailLocation--;
		move = gv->trail[trailLocation];
	}
	else if (move >= DOUBLE_BACK_1 && move <= DOUBLE_BACK_5)
	{
		int back = move - DOUBLE_BACK_1 + 1;
		trailLocation = trailLocation - back;
		move = gv->trail[trailLocation];
	}
	return move;
}

//checks valid transport
bool validTransport(TransportType type, bool road, bool rail, bool boat)
{
	if ((type == ROAD && road) || (type == RAIL && rail) || (type == BOAT && boat))
		return true;
	else 
		return false;
}

//calculates the maximum distance the player can travel by rail
int maxRailDist(Player player, Round round)
{
	//TODO define magic number
	if (player != PLAYER_DRACULA)
		return (player + round) % 4;
	else 
		return 0;
}

//Use depth first search to check valid connections
void dfsReachable(GameView gv, Queue q, PlaceId p, bool road, bool rail, bool boat, 
				int *seen, int dist, int maxDist, bool isDrac)
{
	//base case
	if (dist >= maxDist)
		return;
	seen[p] = 1;
	ConnList connection = MapGetConnections(gv->map, p);
	while (connection != NULL)
	{
		PlaceId n = connection->p;
		if (seen[n] == 0 && validTransport(connection->type, road, rail, boat))
		{
			if (isDrac && n == HOSPITAL_PLACE)
			{
				connection = connection->next;
				continue;
			}
			if (connection->type == RAIL)
			{
				if (isDrac == false)
				{
					seen[n] = 1;
					QueueJoin(q, n);
					dfsReachable(gv, q, n, road, rail, boat, 
							seen, dist + 1, maxDist, isDrac);
				}
			}else if (dist == 0)
			{
				seen[n] = 1;
				QueueJoin(q, n);
			}
		}
		connection = connection->next;
	}
}

PlaceId *trail_location(GameView gv) 
{
	return gv->trail; 
}