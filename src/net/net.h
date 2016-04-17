#include "../shared.h"

#define UPDATE_RATE 100
#define MAXCLIENTS_PER_GAME 4

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>

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