#ifndef _NET_H
#define _NET_H
#pragma once
#include <list>
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
enum CLIENT_REQ {
	SRV_ENTITY_UPDATE = 1,
	CLI_ENTITY_UPDATE,
	GAME_START,
};

typedef struct message_header
{
	CLIENT_REQ reqType;
	unsigned int tick;
	unsigned int entityCount;
	unsigned int payloadLength;
} MessageHeader;

typedef struct entity_update
{
	MessageHeader header;
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
	unsigned int entityCount;
	unsigned int clientCount;
	std::list<st_entity_info>* ents;
	SOCKET clients[MAXCLIENTS_PER_GAME];

} GameState;

int JoinGame(st_entity_info** playerEntities);
st_entity_info* GetEntities(SOCKET c_in, unsigned int* entityCount);
SOCKET connect_to_game();

#endif