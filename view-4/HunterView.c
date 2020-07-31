////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
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

#include "Game.h"
#include "GameView.h"
#include "HunterView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here
#include <limits.h>

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	GameView view;
	int encounter_Dracula;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	HunterView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate HunterView!\n");
		exit(EXIT_FAILURE);
	}
    new->view = GvNew(pastPlays, messages);
    new->encounter_Dracula = 0;
    for(int i = 0; pastPlays[i] != '\0'; i++){
        if(pastPlays[i] == 'V') new->encounter_Dracula = 1;
        break;
    }
	return new;
}

void HvFree(HunterView hv)
{
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv -> view);
}

Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv -> view);
}

int HvGetScore(HunterView hv)
{
	return GvGetScore(hv -> view);
}

int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv -> view, player);
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv -> view, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv -> view);
}

////////////////////////////////////////////////////////////////////////
// Utility Functions

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	// if the Dracula's location is known by hunter

	for (int i = *round; i > 0; i--) {
		if (hv -> encounter_Dracula == 1) {
			PlaceId id = GvGetPlayerLocation(hv -> view, HvGetPlayer(hv));
			return id;
			break;
		}
	}
	// if the Dracula's location is unknown

	return NOWHERE;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
    *pathLength = 0;
	PlaceId current_place = HvGetPlayerLocation(hv, hunter);
	int vertices = MapNumPlaces(hv->view->map);
	//Using Dijkstra's Algorithm to find Single Source Shortest Path
	//Distance Array
	int dist[vertices];
	bool vSet[vertices];

	for (int i = 0; i < vertices; i++) {
		dist[i] = INT_MAX;
		vSet[i] = false;
	}

	dist[current_place] = 0;

	for (int j = 0; j < (vertices - 1); j++) {
		int min_distance_vertex = minDistance(dist, vSet, vertices); 

		vSet[min_distance_vertex] = true;

		for (int k = 0; k < vertices; k++) {
			if (!vSet[k] && hv->view->Map[min_distance_vertex][k] && dist[min_distance_vertex] != INT_MAX) {
				if ((dist[min_distance_vertex] + hv->view->Map[min_distance_vertex][k]) < dist[k]) {
					dist[j] = dist[min_distance_vertex] + hv->view->Map[min_distance_vertex][k]; 
				}

			}
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	if (HvGetPlayerLocation(hv, player) == NOWHERE) {
	    *numReturnedLocs = 0;
	    return NULL;
	}
	else {
	    return GvGetReachable(hv -> view, player, HvGetRound(hv),
                        HvGetPlayerLocation(hv, player), numReturnedLocs);
	
	}
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	if (HvGetPlayerLocation(hv, player) == NOWHERE) {
	    *numReturnedLocs = 0;
	    return NULL;
	}
	else {
	    return GvGetReachableByType(hv -> view, player, HvGetRound(hv),
                        HvGetPlayerLocation(hv, player), road, rail, boat, numReturnedLocs);
	
	}
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	PlaceId current_player_location = HvGetPlayerLocation(hv, player);
	//int num_locations = hv->view->Map->nV;
	//Dynamically Allocated Array to store possible locations
	PlaceId *PossibleLocations = malloc(NUM_REAL_PLACES*sizeof(PlaceId));
	assert(PossibleLocations != NULL);
	//Insert Player's Current Location
	PossibleLocations[0] = current_player_location; 

	//If Player Hasn't Made a Move Yet
	if (hv->view->playerStats[hv->view->player]->moveHistSize == 0) {
		*numReturnedLocs = 0; 
		return NULL;
	}
	//If Given Player is a Hunter
	if (player != PLAYER_DRACULA) {
		//Find available locations
		//Breadth First Search Through All Possibilities
		PossibleLocations = HvFindPossibleLocations(hv, player, true, true, true, numReturnedLocs);

	}
	//If Given Player is Dracula
	if (player == PLAYER_DRACULA) {
		//If Dracula's Current Location is not Revealed
		if (current_player_location == NOWHERE) {
			*numReturnedLocs = 0;
			return NULL;
		} else {
			PossibleLocations = HvFindPossibleLocations(hv, player, true, false, true, numReturnedLocs); 
		}
	}
	
	return PossibleLocations;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	PlaceId current_player_location = HvGetPlayerLocation(hv, player);
	//int num_locations = hv->view->Map->nV;
	//Dynamically Allocated Array to store possible locations
	PlaceId *PossibleLocations = malloc(NUM_REAL_PLACES*sizeof(PlaceId));
	assert(PossibleLocations != NULL);
	//Insert Player's Current Location
	PossibleLocations[0] = current_player_location; 

	//If Player Hasn't Made a Move Yet
	if (hv->view->playerStats[hv->view->player]->moveHistSize == 0) {
		*numReturnedLocs = 0; 
	}
	//If Given Player is a Hunter
	if (player != PLAYER_DRACULA) {
		//Find available locations
		//Breadth First Search Through All Possibilities
		PossibleLocations = HvFindPossibleLocations(hv, player, road, rail, boat, numReturnedLocs);
	}
	//If Given Player is Dracula
	if (player == PLAYER_DRACULA) {
		//If Dracula's Current Location is not Revealed
		if (current_player_location == NOWHERE) {
			*numReturnedLocs = 0;
			return NULL;
		} else {
			PossibleLocations = HvFindPossibleLocations(hv, player, road, rail, boat, numReturnedLocs); 
		}
	}
	return PossibleLocations;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

PlaceId *HvFindPossibleLocations(HunterView hv, Player player,
								   bool road, bool rail, bool boat,
								   int *numReturnedLocs) {
	//int num_vertices = hv->view->Map->nV;
	//int num_edges = hv->view->Map->nE; 
	//Dynamically Allocated Array to store possible locations
	PlaceId *PossibleLocations = malloc(NUM_REAL_PLACES*sizeof(PlaceId));
	assert(PossibleLocations != NULL);
	int possible_locations_array_index = 1;
	PlaceId current_player_location = HvGetPlayerLocation(hv, player);
	PossibleLocations[0] = current_player_location;
	//Get the Connections List for the Player
	ConnList connection_list_player =  MapGetConnections(hv->view->Map, current_player_location); 
	ConnList curr = connection_list_player;
	while (curr != NULL) {
		//If Rail (Dracula Can't Travel By Rail)
		if ((curr->type == RAIL) && (rail == true) && (player != PLAYER_DRACULA)) {
			//Check if can go by Rail on this Segment
			if (HvFindConnectionSingleRound(hv, current_player_location, curr->p, false, true, false) > 0) {
				PossibleLocations[possible_locations_array_index] = curr->p;
			}
			*numReturnedLocs = *numReturnedLocs + 1;
		}
		//If Road
		if ((curr->type == ROAD) && (road == true)) {
			//Check if can go by Road on this Segment
			if (HvFindConnectionSingleRound(hv, current_player_location, curr->p, true, false, false) > 0) {
				//Check if not Hospital of St Joseph & St Mary if Dracula
				if ((player != PLAYER_DRACULA) || ((player == PLAYER_DRACULA) && (curr->p != ST_JOSEPH_AND_ST_MARY))) {
					PossibleLocations[possible_locations_array_index] = curr->p;
				}
			}
			*numReturnedLocs = *numReturnedLocs + 1;
		}
		//If Boat 
		if ((curr->type == BOAT) && (boat == true)) {
			//Check if can go by Boat on this Segment
			if (HvFindConnectionSingleRound(hv, current_player_location, curr->p, false, false, true) > 0) {
				PossibleLocations[possible_locations_array_index] = curr->p;
			}
			*numReturnedLocs = *numReturnedLocs + 1;
		}
		curr = curr->next;
	}
	return PossibleLocations; 
}

int minDistance(int dist[], bool vSet[], int vertices) { 
    // Initialize min value 
    int min = INT_MAX;
	int min_index; 
  
    for (int v = 0; v < vertices; v++) {
        if ((vSet[v] == false) && (dist[v] <= min)) {
            min = dist[v];
			min_index = v; 
		}
	}
    return min_index; 
} 

//Find out Whether Allowed to Travel Between Two Nodes for a Single Round
int HvFindConnectionSingleRound(HunterView hv, PlaceId start, PlaceId end, bool road, bool rail, bool boat) {
	ConnList curr = hv->view->Map->connections[start];
	ConnList destination = hv->view->Map->connections[end]; 
	Player playerId = hv->view->player; 
	Round currentRound = hv->view->round; 
	int distanceRail;
	int numPossibleLocations = 0; 
	while (curr != NULL) {
		//If Rail
		if (curr->type == RAIL && rail == true) {
			//Check allowed distance for rail in current round
			distanceRail = (playerId + currentRound) % 4; 
			//If allowed to travel by rail on 1 segment 
			if (distanceRail == 1) {
				if (curr->p == end) {
					numPossibleLocations++; 
				}
			}
			//If allowed to travel by rail on 2 segments
			if (distanceRail == 2) {
				ConnList middle_segment = hv->view->Map->connections[curr->p];
				if (hv->view->Map->connections[middle_segment->p] == destination) {
					numPossibleLocations++;
				}  
			}
			//If allowed to travel by rail on 3 segments
			if (distanceRail == 3) {
				ConnList first_middle_segment = hv->view->Map->connections[curr->p]; 
				ConnList curr_second_middle_segment = hv->view->Map->connections[first_middle_segment->p]; 
				while (curr_second_middle_segment != NULL) {
					if (curr_second_middle_segment == destination) {
						numPossibleLocations++; 
						break;
					}
					curr_second_middle_segment = curr_second_middle_segment->next; 
				}
			}
		}
		//If Sea
		if (curr->type == BOAT && boat == true) {
			//Check if can Travel By Boat Between Two Cities
			if (curr->p == end)  {
				numPossibleLocations++;
			}
		}
		//If Road
		if (curr->type == ROAD && road == true) {
			//Check if can Travel by Road Between Two Cities
			if (curr->p == end) {
				numPossibleLocations++; 
			}
		}
		curr = curr->next;
	}
	return numPossibleLocations; 
}
