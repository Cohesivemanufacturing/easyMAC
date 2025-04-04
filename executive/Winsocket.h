#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define WAIT_CONNECT      10   // 30 seconds
#define WAIT_RECEIVED     10   // 10 seconds

class Winsocket 
{
public:
	
	/**************************************************************
	Winsocket public services
	***************************************************************/
	Winsocket(int port);						   // Initialize Serial commnications at the specified port
	int isAvailable(void);						   // 
	char* read(void);                       //
	int   write(const std::string &buffer);
	bool  connect(void);                            // waits for a connection on the listening socket

	/**************************************************************
	getters
	***************************************************************/
	inline bool isConnected(void) { return m_isConnected; }  //
	inline bool isCreated(void) { return (ListeningSocket != INVALID_SOCKET); }

	// close the socket
	~Winsocket();

private:
	// winsocks structures
	WSADATA       wsaData;
	SOCKET        ListeningSocket, NewConnection;
	SOCKADDR_IN   ServerAddr, SenderInfo;
	int           port; //Port = 7171;

	// Send part
	int 		  BytesSent;

	// Receiving part
	char          recvbuff[1024];
	int           BytesReceived, SelectTiming;

	// state variables
	bool		  m_isConnected = false;

	/**************************************************************
	Winsocket private members
	***************************************************************/
	int recvTimeOutTCP(SOCKET socket, long sec, long usec);
};