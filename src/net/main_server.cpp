#include <iostream>
#include <thread>
#include "net.h"

typedef int (*handle_callback)(void*, SOCKET);

glm::vec3 position_matrix[] = { glm::vec3{ 0.f,   0.f,    0.f},
								glm::vec3{-40.f,  0.f,   -40.f},
						        glm::vec3{-40.f,  0.f,    40.f},
						        glm::vec3{ 40.f,  0.f,   -40.f},
						        glm::vec3{ 40.f,  0.f,    40.f} };

int test_handle_client(SOCKET c_in)
{
	std::cout << "[=] Made it to client handle with the data" << std::endl;
	return 1;
}

void start_game()
{
	//Initialize game state for clients
	return;

}

int update_client(SOCKET c_in)
{
	//Update game state with client packet
	return 1;

}

int send_client_update(SOCKET c_in)
{
	//Send client updated status based on game
	return 1;

}

int init_client_send(SOCKET c_in)
{
	//Test method
	char* message = "Hey There, this is a test";
	return send(c_in, message, strlen(message), 0);
}

GameState* get_client_game(SOCKET c_in, GameState* gameList)
{
	for(int i = 0; i < MAIN_SRV_MAXGAMES; i++)
	{
		for(int j = 0; j < MAXCLIENTS_PER_GAME; j++)
		{
			if(gameList[i].clients[j] == c_in)
			{
				return &gameList[i];
			}
		}
	}
	return 0;
}

void set_and_send_init_entities(SOCKET c_in, GameState* gameList)
{
	//Get game
	GameState* targetGame = get_client_game(c_in, gameList);
	//Set initial entities based on which client this is.
	//multiplier can be the current number of clients in the game
	st_entity_info tmp_player = {};
	//player.model     = &boxModel;
	tmp_player.scale     = glm::vec3{20.f, 20.f, 20.f};
	tmp_player.position  = position_matrix[targetGame->clientCount];

	targetGame->ents->push_back(tmp_player);
	targetGame->entityCount++;
	MessageHeader* msgHead = (MessageHeader*)malloc(sizeof(MessageHeader) + (sizeof(st_entity_info) * targetGame->entityCount));
	msgHead->reqType = ENTITY_UPDATE;
	msgHead->tick = 0;
	msgHead->entityCount = targetGame->entityCount;
	msgHead->payloadLength = (sizeof(st_entity_info) * targetGame->entityCount);
	st_entity_info* sendData = (st_entity_info*)(msgHead +1);
	for(std::list<st_entity_info>::const_iterator iterator = targetGame->ents->begin();
		iterator != targetGame->ents->end();
		++iterator)
	{
		(*sendData) = *iterator;
		sendData++;
	}
	
	//send it
	int res = send(c_in, (char*)msgHead, (sizeof(MessageHeader) + (sizeof(st_entity_info) * targetGame->entityCount)),0);
	return;
}

int remove_client_from_game(SOCKET c_in, GameState* gameList)
{
	//Look for game this client is a part of, and RIP THEM OUT
	//Also, send update packet to clients that says "Hey, this guy isn't in your game anymore"
	GameState* targetGame = get_client_game(c_in, gameList);
	if(targetGame)
	{
		for(int j = 0; j < MAXCLIENTS_PER_GAME; j++)
		{
			if(targetGame->clients[j] == c_in)
			{
				std::cout << "Removing client " << c_in << " from game" << std::endl;
				targetGame->clients[j]  = 0;
				//TODO: Remove player entities too
				return 1;
			}
		}
	}
	std::cout << "Didn't find client in any games!" << std::endl;
	return 0;
}

int join_client_to_game(SOCKET c_in, GameState* gameList)
{
	//Find first available game
	//Availabe game is
	// - Not Started
	// - Not full
	//Iterate over games and get first game
	int res, addr_size;
	StatusPacket status;
	struct sockaddr_in address;

	res = getpeername(c_in, (sockaddr*)&address, &addr_size);

	std::cout << "Joining Client To Game" << std::endl << "fd: " << c_in << " ip: " << inet_ntoa(address.sin_addr) << " port: " << ntohs(address.sin_port) << std::endl;
	
	for(int i = 0; i < MAIN_SRV_MAXGAMES; i++)
	{
		if((gameList[i].clientCount < MAXCLIENTS_PER_GAME) && (gameList[i].curState == STOPPED))
		{
			//Use this game
			//make sure it's initialized.			
			for(int j = 0; j < MAXCLIENTS_PER_GAME; j++)
			{
				if(!gameList[i].clients[j])
				{
					std::cout << "Successfully Joined client to game " << i << std::endl;
					gameList[i].clients[j] = c_in;
					gameList[i].clientCount++;
					//Send success message to client
					status.status_msg = JOIN_OK;
					res = send(c_in, (char*)&status, sizeof(StatusPacket), 0);
					if(!res)
					{
						std::cout << "Failed to send join status to client" << std::endl;
						return 0;
					}
					//Now send the list of entities we have for this game object.
					set_and_send_init_entities(c_in, gameList);

					return 1;
				}
			}
		}
	}
	//Couldn't find a good game
	status.status_msg = JOIN_FAIL;
	res = send(c_in, (char*)&status, sizeof(StatusPacket), 0);
	return 0;
}

void dispatch_client_data_call(SOCKET c_in, GameState* games)
{

}

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")

void init_game_server(int portnum, SOCKET* master_in)
{
	WSADATA wsa;
	struct sockaddr_in server, address;
	char *message = "Racing Game Main Daemon \r\n";
	 
	//size of our receive buffer, this is string length.
	int MAXRECV = MAIN_SRV_MAXRECV;

	std::cout << "Initializing Winsock" << std::endl;
	if(WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		std::cout << "Failed - Error Code : " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	//Create a socket
	(*master_in) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if((*master_in) == INVALID_SOCKET)
	{
		std::cout << "Couldn't Create Socket: " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	//Prepare sockaddr_in
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portnum);

	//Bind
	if( bind((*master_in), (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Bind Failed: " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	return;
}

int handle_client_event(SOCKET client_socket, GameState* games)
{
	//Call a fucntion based on what the client is recieved and who the client is.
	char *buffer;
	buffer =  (char*) malloc((MAIN_SRV_MAXRECV + 1) * sizeof(char));
	//Read the socket and call the function with the data read or return nothing
	int data = recv(client_socket, buffer, MAIN_SRV_MAXRECV, 0);

	if(data == SOCKET_ERROR)
	{
		int error_code = WSAGetLastError();
		if(error_code == WSAECONNRESET)
		{
			//Disconnect
			std::cout << "Host disconnected: connection reset" << std::endl;
			remove_client_from_game(client_socket, games);
			closesocket(client_socket);
			return 0;
		}
		else
		{
			std::cout << "recv failed with error code " << error_code << std::endl;
			remove_client_from_game(client_socket, games);
			return -1;
		}			
	}
	if(!data)
	{
		//Disconect
		std::cout << "Host disconnected: no data" << std::endl;
		remove_client_from_game(client_socket, games);
		closesocket(client_socket);
	}
	else
	{
		//Call a function based on what we got
		dispatch_client_data_call(client_socket, games);
		return 0;
	}

}

#else


#endif

int main(int argc, char* argv[])
{
	USHORT max_clients = MAIN_SRV_MAXCLIENTS, portnum = MAIN_SRV_DEFAULT_PORTNUM;
	UINT addrlen, activity, res;
	SOCKET master_sock, s, *client_sockets, new_socket;
	fd_set readfds;
	struct sockaddr_in address;
	GameState* currentGames;
	//Maintain a list of game sessions


	//allocate list of sockets
	client_sockets = (SOCKET*)malloc((max_clients) * sizeof(SOCKET));

	//allocate list of games
	currentGames = (GameState*)malloc((MAIN_SRV_MAXGAMES) * sizeof(GameState));

	//clearing out client sockets
	for(int i = 0 ; i < max_clients; i++)
	{
	    client_sockets[i] = 0;
	}

	//Mark all game states as stopped
	for( int i = 0; i < MAIN_SRV_MAXGAMES; i++)
	{
		currentGames[i].curState = STOPPED;
		currentGames[i].serverTicks = 0;
		currentGames[i].clientCount = 0;
		currentGames[i].entityCount = 0;
		currentGames[i].ents = new std::list<st_entity_info>();
		for(int j = 0; j < MAXCLIENTS_PER_GAME; j++)
		{
			currentGames[i].clients[j] = 0;
		}
	}

	//Call Init
	init_game_server(portnum, &master_sock);

	//Listen
	listen(master_sock, 3);

	std::cout << "Waiting for incomming connections.." << std::endl;

	addrlen = sizeof(struct sockaddr_in);

	while(TRUE)
	{
		FD_ZERO(&readfds);
		FD_SET(master_sock, &readfds);

		//adding child FDs to client set		
		for(int i = 0; i < max_clients; i++)
		{
			if(client_sockets[i] > 0)
			{
				FD_SET(client_sockets[i], &readfds);
			}			
		}

		activity = select(0, &readfds, NULL, NULL, NULL);

		if(activity == SOCKET_ERROR)
		{
			std::cout << "Select call failed with error code " << WSAGetLastError() << std::endl;
			exit(EXIT_FAILURE);
		}

		//We got a master_socket request, or a new connection
		if(FD_ISSET(master_sock, &readfds))
		{
			if((new_socket = accept(master_sock, (struct sockaddr*)&address, (int *)&addrlen)) < 0)
			{
				std::cout << "Error accepting socket" << std::endl;
				exit(EXIT_FAILURE);
			}

			std::cout << "Got new connection: fd: " << new_socket << " ip: " << inet_ntoa(address.sin_addr) << " port: " << ntohs(address.sin_port) << std::endl;
			
			//Send init message
			//This should join the client with the first available game instance
			//res = init_client_send(new_socket);
			res = join_client_to_game(new_socket, currentGames);

			//add new connection
			for(int i = 0; i < max_clients; i++)
			{
				if(client_sockets[i] == 0)
				{
					client_sockets[i] = new_socket;
					std::cout << "Adding to list of sockets at index: " << i << std::endl;
					break;
				}
			}
		}

		//Else action on a client
		for(int i = 0; i < max_clients; i++)
		{
			s = client_sockets[i];
			if(FD_ISSET(s, &readfds))
			{				
				res = handle_client_event(s, currentGames);
				if(!res || res < 0)
				{
					//Disconnection from handler, lets set the client socket to zero.
					client_sockets[i] = 0;
				}				
			}
		}
	}

	return 0;
}