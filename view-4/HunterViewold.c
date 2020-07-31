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

// TODO: ADD YOUR OWN STRUCTS HERE

struct hunterView {
	GameView view;
	int encounter_Dracula;
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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

	for(int i = *round; i > 0; i--){
		if(hv -> encounter_Dracula == 1){
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
	PlaceId current_place = hv->view->trail[0];
	int vertices = map->nV;
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
		int min_distance_vertex = minDistance(dist, vSet); 

		vSet[min_distance_vertex] = true;

		for (int k = 0; k < vertices; k++) {
			if (!vSet[k] && map[min_distance_vertex][k] && dist[min_distance_vertex] != INT_MAX) {
				if ((dist[min_distance_vertex] + ) < dist[k]) {
					dist[v] = dist[min_distance_vertex] + map[min_distance_vertex][k]; 
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
	if(HvGetPlayerLocation(hv, player) == NOWHERE){
	    *numReturnedLocs = 0;
	    return NULL;
	}
	else{
	    return GvGetReachable(hv -> view, player, HvGetRound(hv),
                        HvGetPlayerLocation(hv, player), numReturnedLocs);
	
	}
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	Player player = HvGetPlayer(hv);
	if(HvGetPlayerLocation(hv, player) == NOWHERE){
	    *numReturnedLocs = 0;
	    return NULL;
	}
	else{
	    return GvGetReachableByType(hv -> view, player, HvGetRound(hv),
                        HvGetPlayerLocation(hv, player), road, rail, boat, numReturnedLocs);
	
	}
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	PlaceId current_player_location = HvGetPlayerLocation(hv, player);
	int num_locations = map->nV;
	//Dynamically Allocated Array to store possible locations
	PossibleLocations = malloc(num_locations*sizeof(PlaceId));
	assert(PossibleLocations != NULL);
	//Insert Player's Current Location
	PossibleLocations[0] = current_player_location; 

	//If Player Hasn't Made a Move Yet
	if (player->moveHistSize == 0) {
		*numReturnedLocs = 0; 
		return NULL;
	}
	//If Given Player is a Hunter
	if (player != PLAYER_DRACULA) {
		//Find available locations
		//Breadth First Search Through All Possibilities
		PossibleLocations = FindPossibleLocations(hv, player, true, true, true, numReturnedLocs);

	}
	//If Given Player is Dracula
	if (player == PLAYER_DRACULA) {
		//If Dracula's Current Location is not Revealed
		if (current_player_location == NULL) {
			*numReturnedLocs = 0;
			return NULL;
		} else {
			PossibleLocations = FindPossibleLocations(hv, player, true, false, true, numReturnedLocs); 
		}
	}
	
	return PossibleLocations;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	PlaceId current_player_location = HvGetPlayerLocation(hv, player);
	int num_locations = map->nV;
	//Dynamically Allocated Array to store possible locations
	PossibleLocations = malloc(num_locations*sizeof(PlaceId));
	assert(PossibleLocations != NULL);
	//Insert Player's Current Location
	PossibleLocations[0] = current_player_location; 

	//If Player Hasn't Made a Move Yet
	if (player->moveHistSize == 0) {
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
		if (current_player_location == NULL) {
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

PlaceId *FindPossibleLocations(HunterView hv, Player player,
								   bool road, bool rail, bool boat,
								   int *numReturnedLocs) {
	int num_vertices = map->nV;
	int num_edges = map->nE; 
	//Dynamically Allocated Array to store possible locations
	PossibleLocations = malloc(num_locations*sizeof(PlaceId));
	assert(PossibleLocations != NULL);
	int possible_locations_array_index = 0;
	PlaceId current_player_location = HvGetPlayerLocation(hv, player);
	//Get the Connections List for the Player
	ConnList connection_list_player =  MapGetConnections(map, current_player_location); 
	ConnList curr = connection_list_player;
	while (curr != NULL) {
		//If Rail
		if ((curr->type == RAIL) && (rail == true)) {
			//Check if can go by Rail on this Segment
			if (HvFindConnectionSingleRound(hv, curr_player_location, curr->p, false, true, false)) {
				PossibleLocations[possible_locations_array_index] = curr->p;
			}
			int *numReturnedLocs++;
		}
		//If Road
		if ((curr->type == ROAD) && (road == true)) {
			//Check if can go by Road on this Segment
			if (HvFindConnectionSingleRound(hv, curr_player_location, curr->p, true, false, false)) {
				PossibleLocations[possible_locations_array_index] = curr->p;
			}
			int *numReturnedLocs++;
		}
		//If Boat 
		if ((curr->type == BOAT) && (boat == true)) {
			//Check if can go by Boat on this Segment
			if (HvFindConnectionSingleRound(hv, curr_player_location, curr->p, false, false, true)) {
				PossibleLocations[possible_locations_array_index] = curr->p;
			}
			int *numReturnedLocs++;
		}
		curr = curr->next;
	}
	return PossibleLocations; 
}

int minDistance(int dist[], bool vSet[]) { 
    // Initialize min value 
    int min = INT_MAX;
	int min_index; 
  
    for (int v = 0; v < V; v++) {
        if ((vSet[v] == false) && (dist[v] <= min)) {
            min = dist[v];
			min_index = v; 
		}
	}
    return min_index; 
} 

//Find out Whether Allowed to Travel Between Two Nodes for a Single Round
int HvFindConnectionSingleRound(HunterView hv, PlaceId start, PlaceId end, bool road, bool rail, bool boat) {
	ConnList curr = map->connections[start];
	ConnList destination = map->connections[end]; 
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
				ConnList middle_segment = map->connections[curr->p];
				if (map->connections[middle_segment->p] == destination) {
					numPossibleLocations++;
				}  
			}
			//If allowed to travel by rail on 3 segments
			if (distanceRail == 3) {
				ConnList first_middle_segment = map->connections[curr->p]; 
				ConnList curr_second_middle_segment = map->connections[0]; 
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