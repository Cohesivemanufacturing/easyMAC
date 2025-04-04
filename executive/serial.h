#pragma once

#include <windows.h> 

#include <string>
#include <stdio.h>
#include <stdlib.h>

#define WAIT_TIME 2000
#define COMPORT_STRLENGTH 16
#ifndef MAX_LEN   
#define MAX_LEN 100
#endif

class Serial
{
private:
	//Serial comm handler
	HANDLE hSerial;
	//Connection status
	bool connected;
	//Get various information about the connection
	COMSTAT status;
	//Keep track of last error
	DWORD errors;
	// COM port name
	TCHAR COMPORT[COMPORT_STRLENGTH];
	// a bufer for the incomming messages
	char m_buffer[MAX_LEN];  
	

public:
	//Initialize Serial communication with the given COM port
	Serial(char *portName, int baud);
	//Close the connection
	~Serial();
	
	/* Read data in a buffer, if nbChar is greater than the
	 * maximum number of bytes available, it will return only the
	 * bytes available. The function return -1 when nothing could
	 * be read, the number of bytes actually read.
	 */
	int read(char *buffer, unsigned int nbChar);

	/*
	 * reads data in a buffer until new-line character '\n'.
	 */
	char* readln(void) ;
	
	/*
	* Checks if there is something to read.
	*/
	int available(void);

	/*Writes data from a buffer through the Serial connection
	 *return true on success.
	 */
	bool write(char *buffer, int nbChar);

	
	/**************************************************************
	getters
	***************************************************************/
	
	inline bool isConnected(void) { return this->connected; }
	
	inline bool isCOMportAvailable(void)  
	{
		TCHAR buf[64];
		TCHAR COM_substr[8];
		strncpy(COM_substr, COMPORT + 4, 6);

		int len = QueryDosDevice(COM_substr, buf, 64);

		if (len > 0)
			return true;
		else
			return false;
	}
	
	/**************************************************************
	setters
	***************************************************************/
};

