#include <iostream>

#define MAIN_SRV_MAXRECV 1024
#define MAIN_SRV_MAXCLIENTS 30
#define MAIN_SRV_DEFAULT_PORTNUM 21825

typedef int (*handle_callback)(void*);

int test_handle_client(void* input)
{
	std::cout << "[=] Made it to client handle with the data" << std::endl;
	return 1;
}

#ifdef _WIN32

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")

int init_client_send(SOCKET sin)
{
	//Send something on this socket.
	char* message = "Hey There, this is a test";
	return send(sin, message, strlen(message), 0);
}


void init_game_server(int portnum, SOCKET* master_in, USHORT num_sockets)
{
	WSADATA wsa;
	struct sockaddr_in server, address;
	int max_clients = num_sockets;
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

int handle_client_event(SOCKET client_socket, handle_callback hFunc)
{
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
			closesocket(client_socket);
			return 0;
		}
		else
		{
			std::cout << "recv failed with error code " << error_code << std::endl;
			return -1;
		}			
	}
	if(!data)
	{
		//Disconect
		std::cout << "Host disconnected: no data" << std::endl;
		closesocket(client_socket);
	}
	else
	{
		return hFunc((void*)&buffer);
	}

}

#else

void init_game_server(int portnum, SOCKET* master_in, SOCKET* client_sockets, USHORT num_sockets)
{
	WSADATA wsa;
	//SOCKET master , new_socket, s;
	struct sockaddr_in server, address;
	int max_clients = num_sockets, activity, addrlen, i, valread;
	char *message = "Racing Game Main Daemon \r\n";
	 
	//size of our receive buffer, this is string length.
	int MAXRECV = MAIN_SRV_MAXRECV;

	//set of socket descriptors
	fd_set readfds;

	//1 extra for null character, string termination
	char *buffer;
	buffer =  (char*) malloc((MAXRECV + 1) * sizeof(char));

	//clearing out client sockets
	for(i = 0 ; i < max_clients; i++)
	{
	    client_sockets[i] = 0;
	}

	std::cout << "Initializing Winsock" << std::endl;
	if(WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		std::cout << "Failed - Error Code : " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	//Create a socket
	if((*master_in) = socket(AF_INET, SOCK_STREAM, 0) == INVALID_SOCKET)
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

}

#endif

int main(int argc, char* argv[])
{
	USHORT max_clients = MAIN_SRV_MAXCLIENTS, portnum = MAIN_SRV_DEFAULT_PORTNUM;
	UINT addrlen, activity, res;
	SOCKET master_sock, s, *client_sockets, new_socket;
	fd_set readfds;
	struct sockaddr_in address;

	//allocate list of sockets
	client_sockets = (SOCKET*)malloc((max_clients) * sizeof(SOCKET));

	//clearing out client sockets
	for(int i = 0 ; i < max_clients; i++)
	{
	    client_sockets[i] = 0;
	}

	//Call Init
	init_game_server(portnum, &master_sock, max_clients);

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
			res = init_client_send(new_socket);

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
				getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);

				res = handle_client_event(s, test_handle_client);
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