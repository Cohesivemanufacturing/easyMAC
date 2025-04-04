#include "Winsocket.h"

/**************************************************************
Static functions outside the class
***************************************************************/

/**************************************************************
Winsocks object
***************************************************************/

// public services

Winsocket::Winsocket(int port)
{
	// Initialize Winsock version 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		// The WSAGetLastError() function is one of the only functions
		// in the Winsock 2.2 DLL that can be called in the case of a WSAStartup failure
		printf("Server: WSAStartup failed with error %ld.\n", WSAGetLastError());
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		//Tell the user that we could not find a usable WinSock DLL
		printf("Server: The dll do not support the Winsock version %u.%u!\n",
			LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
		// Do the clean up
		WSACleanup();
	}

	// Create a new socket to listen for client connections.
	ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	if (ListeningSocket == INVALID_SOCKET)
	{
		printf("Server: Error at socket(), error code: %ld.\n", WSAGetLastError());
		// Clean up
		WSACleanup();
	}

	// Set up a SOCKADDR_IN structure that will tell bind that we
	// want to listen for connections on all interfaces using port 7171.

	// The IPv4 family
	ServerAddr.sin_family = AF_INET;
	// host-to-network byte order
	ServerAddr.sin_port = htons(port);
	// Listen on all interface, host-to-network byte order
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Associate the address information with the socket using bind.
	// Call the bind function, passing the created socket and the sockaddr_in
	// structure as parameters. Check for general errors.
	if (bind(ListeningSocket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("Server: bind() failed! Error code: %ld.\n", WSAGetLastError());
		// Close the socket
		closesocket(ListeningSocket);
		// Do the clean up
		WSACleanup();
	}

	// Listen for client connections with a backlog of 5
	if (listen(ListeningSocket, 5) == SOCKET_ERROR)
	{
		printf("Server: listen(): Error listening on socket %ld.\n", WSAGetLastError());
		// Close the socket
		closesocket(ListeningSocket);
		// Do the clean up
		WSACleanup();
	}
}

Winsocket::~Winsocket()
{
	// When all the data communication and listening finished, close the socket
	if (closesocket(ListeningSocket) != 0)
		printf("Server: Cannot close \"ListeningSocket\" socket. Error code: %ld\n", WSAGetLastError());
	else
		printf("Server: Closing \"ListeningSocket\" socket...\n");

	// Finally and optionally, clean up all those WSA setup
	if (WSACleanup() != 0)
		printf("Server: WSACleanup() failed! Error code: %ld\n", WSAGetLastError());
	else
		printf("Server: WSACleanup() is OK...\n");
}

bool Winsocket::connect(void)
{
	
	// Set 60 timeout
	SelectTiming = recvTimeOutTCP(ListeningSocket, WAIT_CONNECT, 0); // waits for a conection and returns an output

	switch (SelectTiming)
	{
	case 0:
		// Timed out, do whatever you want to handle this situation

		break;
	case -1:
		// Error occurred, more tweaking here and the recvTimeOutTCP()...
		printf("\nServer: Some error encountered with code number: %ld\n", WSAGetLastError());
		break;
	default:
	{
		// Accept connection on the ListeningSocket socket and assign
		// it to the NewConnection socket, let the ListeningSocket
		// do the listening for more connection
		NewConnection = accept(ListeningSocket, NULL, NULL);		
		m_isConnected = true;
	}
	}

	return m_isConnected;
}

int Winsocket::isAvailable(void) 
{
	BytesReceived = recv(NewConnection, recvbuff, sizeof(recvbuff), 0);

	// Check for errors in the connection
	if (BytesReceived == SOCKET_ERROR)
	{
		m_isConnected = false;
		return 0;
	}

	return BytesReceived;
}

char* Winsocket::read(void)
{
	recvbuff[BytesReceived] = '\0';
	return recvbuff;
}

int Winsocket::write(const std::string &buffer)
{
	
	BytesSent =  send(NewConnection, buffer.c_str(), buffer.length(), 0);
	
	// Check for errors in the connection
	if (BytesSent == SOCKET_ERROR)
	{
		m_isConnected = false;
		//printf("Server.send: client got disconnected\n\n>");
	}

	return BytesSent;
}

int Winsocket::recvTimeOutTCP(SOCKET socket, long sec, long usec)
{
	// Setup timeval variable
	struct timeval timeout;   // The maximum time for select to wait, provided in the form of a TIMEVAL structure.
	struct fd_set fds;        // A pointer to a set of sockets to be checked for readability. 

							  // assign the second and microsecond variables
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;
	// Setup fd_set structure
	FD_ZERO(&fds);			 // Initializes the set to the null set.	
	FD_SET(socket, &fds);    // Adds descriptor s to set.

							 // select function: determines the status of one or more sockets, waiting if necessary, 
							 // to perform synchronous I/O.
							 // Possible return values:
							 // -1: error occurred
							 // 0: timed out
							 // > 0: data ready to be read
	return select(0, &fds, 0, 0, &timeout);
}

