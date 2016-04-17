#ifndef _NET_H
#define _NET_H
#pragma once
#include "../shared.h"

#define UPDATE_RATE 100
#define MAXCLIENTS_PER_GAME 4
#define DEFAULT_SERVER_HOST "127.0.0.1"
#define MAIN_SRV_MAXRECV 1024
#define MAIN_SRV_MAXCLIENTS 28
#define MAIN_SRV_MAXGAMES 7
#define MAIN_SRV_DEFAULT_PORTNUM 21825

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>

int init_winsock();

#endif

enum GAME_STATES {STARTED=1, STOPPED};
enum STATUS_MESSAGE { JOIN_OK=1, JOIN_FAIL};

typedef struct entity_list
{
	st_entity* ent;
	st_entity* prev;
	st_entity* next;
} EntityList;

typedef struct entity_update
{
	unsigned int tick;
	unsigned int entityCount;
	void* update_data; //This might have to be more defined later. but probably a position and orientation vector
} EntityUpdate;

typedef struct status_packet
{
	//Could be more stuff here
	STATUS_MESSAGE status_msg;
} StatusPacket;

typedef struct game_state
{
	//Maintains list of entities and tick count.
	GAME_STATES curState;
	unsigned int serverTicks;
	EntityList ents;
	int clientCount;
	SOCKET clients[MAXCLIENTS_PER_GAME];

} GameState;

int JoinGame(EntityList* playerEntities);
SOCKET connect_to_game();

#endif