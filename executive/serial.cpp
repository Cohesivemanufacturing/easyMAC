#include "serial.h"

/*
* lpct_strcpty
* Jorge Correa 2017
*/
static inline void lpct_strcpy(TCHAR *dst, unsigned int src_length, char* src) 
{
	char* pnt0 = src;

	for (char* pnt = pnt0; pnt != pnt0 + src_length; pnt++)
	{
		*dst++ = *pnt;
	}
}

Serial::Serial(char *portName, int baud)
{
	//We're not yet connected
	this->connected = false;

	//Try to connect to the given port throuh CreateFile
	
	//this->hSerial = CreateFileA(portName,
	//	GENERIC_READ | GENERIC_WRITE,
	//	0,
	//	NULL,
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL);

	this->hSerial = CreateFile(portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// stores the COM port name i
	
	// lpct_strcpy(COM, COMPORT_STRLENGTH, portName);
	
	strcpy(COMPORT, portName);

	//Check if the connection was successfull
	if (this->hSerial == INVALID_HANDLE_VALUE)
	{
		//If not success full display an Error
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {

			//Print Error if neccessary
			printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			//If impossible, show an error
			printf("failed to get current serial parameters!");
		}
		else
		{
			//Define serial connection parameters for the arduino board
			//dcbSerialParams.BaudRate = CBR_115200;
			dcbSerialParams.BaudRate = baud;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			//Setting the DTR to Control_Enable ensures that the Arduino is properly
			//reset upon establishing a connection
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				printf("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				//If everything went fine we're connected
				this->connected = true;
				//Flush any remaining characters in the buffers 
				PurgeComm(this->hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				//We wait 2s as the arduino board will be reseting
				Sleep(WAIT_TIME);
			}
		}
	}

}

Serial::~Serial()
{
	//Close the serial handler
	CloseHandle(this->hSerial);
}

int Serial::read(char *buffer, unsigned int nbChar) 
{
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	unsigned int toRead;

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &this->errors, &this->status);

	//Check if there is something to read
	if (this->status.cbInQue>0)
	{
		//If there is we check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if (this->status.cbInQue>nbChar)
		{
			toRead = nbChar;
		}
		else
		{
			toRead = this->status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL))
		{
			return bytesRead;
		}

	}

	//If nothing has been read, or that an error was detected return 0
	return 0;

}

/*
* readln
* Jorge Correa 2017
*/

char* Serial::readln(void) 
{
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	char tempChar;
	// a counter to index the buffer 
	char* ptr = m_buffer;

	do
	{
		ReadFile(this->hSerial,       //Handle of the Serial port
			&tempChar,        //Temporary character
			sizeof(tempChar), //Size of TempChar
			&bytesRead,       //Number of bytes read
			NULL);
		if (bytesRead > 0) {
			*ptr++ = tempChar;			  //store Tempchar into buffer
		}
	} while ((tempChar != '\n'));

	*ptr = 0; // finishes the message
	
	return m_buffer;
}

/*
* available
* Jorge Correa 2017
*/
int Serial::available(void)
{

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &this->errors, &this->status);

	//Check if there is something to read and returns
	return this->status.cbInQue;
};

bool Serial::write(char *buffer, int nbChar)
{
	DWORD bytesSend;

	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
	{
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);

		return false;
	}
	else
		return true;
}
