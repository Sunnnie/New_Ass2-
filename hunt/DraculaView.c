////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here

// TODO: ADD YOUR OWN STRUCTS HERE
struct draculaView {
	// TODO: ADD FIELDS HERE
	GameView gv; 
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}


	new->gv = GvNew(pastPlays, messages); 

	return new;
}

void DvFree(DraculaView dv)
{

	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	GvFree(dv->gv);

	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	//*numTraps = trapLocationSize(dv->gv);
	
	return GvGetTrapLocations(dv->gv, numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move
//cases: 
PlaceId whereIs(DraculaView currentView, Player player)
{
    int trueLocation = GvGetPlayerLocation(currentView->gv, PLAYER_DRACULA); //retrieves given player's location from GameView
   
    PlaceId *dracTrail = trail_location(currentView->gv);

    switch (trueLocation) {
        case TELEPORT: trueLocation = CASTLE_DRACULA; break;
        case DOUBLE_BACK_1: trueLocation = dracTrail[1]; break;
        case DOUBLE_BACK_2: trueLocation = dracTrail[2]; break;
        case DOUBLE_BACK_3: trueLocation = dracTrail[3]; break;
        case DOUBLE_BACK_4: trueLocation = dracTrail[4]; break;
        case DOUBLE_BACK_5: trueLocation = dracTrail[5]; break;
        default: ;//Do nothing;
    }
    return trueLocation;
}

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
    
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION 
	PlaceId *vaild_moves = malloc(NUM_REAL_PLACES*sizeof(PlaceId)); 
	(*numReturnedMoves) = 0;
	
	if(GvGetRound(dv->gv) == 0) {
	    for (int a = 0; a < NUM_REAL_PLACES; a++) {
	        if (a != ST_JOSEPH_AND_ST_MARY) {
	            vaild_moves[(*numReturnedMoves)] = a;
	            (*numReturnedMoves)++; 
	        }
	    }
	} else {
	    
	    int hist = -1; 
	    bool canFree = true; 
	    PlaceId *hist_move = GvGetMoveHistory(dv->gv, PLAYER_DRACULA, &hist, &canFree);  
	    
	    int hidden = 0;
	    int double_back = 0; 
	    
	    if (placeIdToType(whereIs(dv, PLAYER_DRACULA)) == SEA) {
			hidden = 1; 
		}
		
		if (hist > 4) {
		    for (int i = hist - 1; i >= hist - 5; i--) {
		        if (hist_move[i] >= DOUBLE_BACK_1 && hist_move[i] <= DOUBLE_BACK_5) {
		            double_back = 1; 
		        }
		        
		        if (hist_move[i] == HIDE) hidden = 1;  
		    } 
		} else {
		    for (int i = 0; i < hist; i++) {
		        if (hist_move[i] >= DOUBLE_BACK_1 && hist_move[i] <= DOUBLE_BACK_5) {
		            double_back = 1;
		    } 
		    
		    if (hist_move[i] == HIDE) hidden = 1;
		    }  
		}
		
	    int num_reachable = -1;
	    PlaceId *reachable = malloc(sizeof(PlaceId)*NUM_REAL_PLACES); 
	    reachable = GvGetReachable(dv->gv, PLAYER_DRACULA, GvGetRound(dv->gv), GvGetPlayerLocation(dv->gv, PLAYER_DRACULA), &num_reachable); 
	    for (int i = num_reachable - 1; i > 0; i--) {
	        vaild_moves[(*numReturnedMoves)] = reachable[i];
	        (*numReturnedMoves)++; 
	    } 
	    
      
	    if (hidden == 0) {
            vaild_moves[(*numReturnedMoves)] = HIDE; 
            (*numReturnedMoves)++; 
        } 		
		
		int back = DOUBLE_BACK_1; 
		if (double_back == 0) {
		    if (hist < 5) {
		        for (int i = 0; i < hist; i++) {
		            vaild_moves[(*numReturnedMoves)] = back; 
		            (*numReturnedMoves)++; 
		            back++; 
		        } 
		    } else {
		        for (int j = 0; j < 5; j++) {
		            vaild_moves[(*numReturnedMoves)] = back; 
		            (*numReturnedMoves)++; 
		            back++;
		        } 
		    }  
		} 

    }
	return vaild_moves;  

}



PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
    PlaceId *vaild_locations = malloc(NUM_REAL_PLACES*sizeof(PlaceId));
	(*numReturnedLocs) = 0;
	if(GvGetRound(dv->gv) == 0) {
	    for (int a = 0; a < NUM_REAL_PLACES; a++) {
	        if (a != ST_JOSEPH_AND_ST_MARY) {
	            vaild_locations[(*numReturnedLocs)] = a;
	            (*numReturnedLocs)++; 
	        }
	    }
	} else {
	    PlaceId *trail = trail_location(dv->gv); 
	    int a = 0;
	    while(trail[a] != NOWHERE) {
	        a++; 
	    } 

	    //checking for duplicate 
	    int num_reachable = -1;
	    PlaceId *reachable = malloc(sizeof(PlaceId)*NUM_REAL_PLACES); 
	    reachable = GvGetReachable(dv->gv, PLAYER_DRACULA, GvGetRound(dv->gv), whereIs(dv, PLAYER_DRACULA), &num_reachable);               
	    for (int i = num_reachable; i > 0; i--) {
	        int flag = 0;
	        for (int j = a-1; j >= 0; j--) {
	            if (reachable[i] == trail[j]) flag = 1; 	                     
            }
            if (flag == 0) {
            vaild_locations[(*numReturnedLocs)] = reachable[i];
            (*numReturnedLocs)++;            
            } 
        }	                	    

	    //add current location if HIDE is available && add double_back locations from trails
	          	    
	    int temp = a-1; 	
	    int move;   
	    
	    PlaceId *move_hist = DvGetValidMoves(dv, &move); 
	    for (int i = 0; i < move; i++) {
	        if (move_hist[i] == HIDE) {
	            vaild_locations[(*numReturnedLocs)] = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA); 
	            (*numReturnedLocs)++;  
	        } else if (move_hist[i] >= DOUBLE_BACK_1 && move_hist[i] <= DOUBLE_BACK_5) {
	            vaild_locations[(*numReturnedLocs)] = trail[temp];
	            (*numReturnedLocs)++; 
	            temp--;  
	        } 
	    } 
	    
	    
	} 
     
    return vaild_locations; 
}


PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
    PlaceId *vaild_locations = malloc(NUM_REAL_PLACES*sizeof(PlaceId));
	(*numReturnedLocs) = 0;
	if(GvGetRound(dv->gv) == 0) {
	    for (int a = 0; a < NUM_REAL_PLACES; a++) {
	        if (a != ST_JOSEPH_AND_ST_MARY) {
	            vaild_locations[(*numReturnedLocs)] = a;
	            (*numReturnedLocs)++; 
	        }
	    }
	} else {
	    PlaceId *trail = trail_location(dv->gv); 
	    int a = 0;
	    while(trail[a] != NOWHERE) {
	        a++; 
	    } 

	    //checking for duplicate 
	    int num_reachable = -1;
	    PlaceId *reachable = malloc(sizeof(PlaceId)*NUM_REAL_PLACES); 
	    reachable = GvGetReachableByType(dv->gv, PLAYER_DRACULA, GvGetRound(dv->gv), GvGetPlayerLocation(dv->gv, PLAYER_DRACULA), true, false, true,  &num_reachable);               
	    for (int i = 0; i < num_reachable; i++) {
	        int flag = 0;
	        for (int j = a-1; j >= 0; j--) {
	            if (reachable[i] == trail[j]) flag = 1; 	                     
            }
            if (flag == 0) {
            vaild_locations[(*numReturnedLocs)] = reachable[i];
            (*numReturnedLocs)++;            
            } 
        }	                	    

	    //add current location if HIDE is available && add double_back locations from trails
	           	    
	    int temp = a-1; 	
	    int move;   
	    
	    PlaceId *move_hist = DvGetValidMoves(dv, &move); 
	    for (int i = 0; i < move; i++) {
	        if (move_hist[i] == HIDE) {
	            vaild_locations[(*numReturnedLocs)] = GvGetPlayerLocation(dv->gv, PLAYER_DRACULA); 
	            (*numReturnedLocs)++;  
	        } else if (move_hist[i] >= DOUBLE_BACK_1 && move_hist[i] <= DOUBLE_BACK_5) {
	            vaild_locations[(*numReturnedLocs)] = trail[temp];
	            (*numReturnedLocs)++; 
	            temp--;  
	        } 
	    } 
	    
	    
	} 
     
    return vaild_locations; 

}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *reachable = malloc(NUM_REAL_PLACES*sizeof(PlaceId)); 
	*numReturnedLocs = 0; 

	if(player == PLAYER_DRACULA) {
		reachable =  DvWhereCanIGo(dv, numReturnedLocs);
	} else {

		Round next_move; 

		if (player >= GvGetPlayer(dv->gv)) {
			next_move = GvGetRound(dv->gv); //next move happen in this round, not his turn yet 
		} else {
			next_move = GvGetRound(dv->gv) + 1; 
		}

		if (next_move == 0) { //first round
			for (int a = 0; a < NUM_REAL_PLACES; a++) {
				reachable[*numReturnedLocs] = a;
				(*numReturnedLocs)++; 
			}
		}  else {
			reachable = GvGetReachable(dv->gv, player, next_move, GvGetPlayerLocation(dv->gv, player), numReturnedLocs); 
		}
	}
		
	return reachable;
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *reachable_type = malloc(NUM_REAL_PLACES*sizeof(PlaceId)); 
	*numReturnedLocs = 0; 
	
	if(player == PLAYER_DRACULA) {
		reachable_type =  DvWhereCanIGoByType(dv, road, boat, numReturnedLocs);
	} else {

		Round next_move; 

		if (player >= GvGetPlayer(dv->gv)) {
			next_move = GvGetRound(dv->gv); //next move happen in this round, not his turn yet 
		} else {
			next_move = GvGetRound(dv->gv) + 1; 
		}

		if (next_move == 0) { //first round
			for (int a = 0; a < NUM_REAL_PLACES; a++) {
				reachable_type[*numReturnedLocs] = a;
				(*numReturnedLocs)++; 
			}
		}  else {
			reachable_type = GvGetReachableByType(dv->gv, player, next_move, GvGetPlayerLocation(dv->gv, player), road, rail, boat, numReturnedLocs); 
		}
	}	
	return reachable_type;
}
