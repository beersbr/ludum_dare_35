#include "net.h"
#ifdef _WIN32

int init_game_winsock()
{
	//Connect to server and return socket
	WSADATA WsaDat;
    if(WSAStartup(MAKEWORD(2,2),&WsaDat)!=0)
    {
        std::cout<<"Winsock error - Winsock initialization failed\r\n";
        WSACleanup();
        system("PAUSE");
        return 0;
    }

    return 1;
}

// Create our socket
SOCKET connect_to_game()
{
	SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(Socket==INVALID_SOCKET)
	{
		std::cout << "Winsock error - Socket creation Failed: " << WSAGetLastError() << std::endl;
	    WSACleanup();
	    system("PAUSE");
	    return 0;
	}

	// Resolve IP address for hostname
	struct hostent *host;
	if((host=gethostbyname(DEFAULT_SERVER_HOST))==NULL)
	{
	    std::cout<<"Failed to resolve hostname.\r\n";
	    WSACleanup();
	    system("EXIT");
	    return 0;
	}

	// Setup our socket address structure
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port=htons(MAIN_SRV_DEFAULT_PORTNUM);
	SockAddr.sin_family=AF_INET;
	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);

	// Attempt to connect to server
	if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr))!=0)
	{
	    std::cout<<"Failed to establish connection with server\r\n";
	    WSACleanup();
	    system("EXIT");
	    return 0;
	}
	return Socket;
}

#endif

int JoinGame(EntityList* playerEntities)
{
	//Attempt to join default server host and get player entities.
	//If a failure message is recieved, just dont give a model I suppose
	//Server should dictate where the models get drawn initially
	char recvBuf[MAIN_SRV_MAXRECV];
	StatusPacket* joinStat = 0;
	int res;

	SOCKET mySocket = connect_to_game();
	if(!mySocket)
	{
		std::cout << "Couldn't connect to game server..." << std::endl;
		return 0;
	}

	//If we did connect, we should probably recv whether or not we joined
	res = recv(mySocket, recvBuf, sizeof(StatusPacket), 0);
	joinStat = (StatusPacket*)recvBuf;

	if(joinStat->status_msg == JOIN_OK)
	{
		std::cout << "Joined Game Successfully. Getting Player Entitites" << std::endl;
	}
	else
	{
		std::cout << "Failed to join game." << std::endl;
		return 0;
	}

	return 1;
}