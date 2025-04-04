#pragma once
/* Buffer parameters */

#define MAX_BUF  50				  // max. incoming buffer size
#define MAX_LEN  60


class Buffer
{
public:

	int  size = MAX_BUF;           // the size of the queue
	int  lenght = MAX_LEN;         // define memory space for the queue
	char buffer[MAX_BUF][MAX_LEN];
	int  n_elem;
	int  countin;			       // an index for the in element in the queue
	int  countout;			       // an index for the out element in the queue
	int  countback;

	Buffer() { countin = countout = countback = n_elem = 0; };

	virtual ~Buffer() {};

	int   write(char* block) volatile;
	char* read(void) volatile;

	/**************************************************************
	getters
	***************************************************************/
	inline int  numElements(void) volatile { return n_elem; }
	inline bool isEmpty(void) volatile	{ return (countin == countout) ? true : false; }
	//inline bool isFull(void) volatile { return (n_elem >= size - 1); }
	inline bool isFull(void) volatile { return countin == ((countout - 1 + size) % size); }
	inline bool isSynchronized(void) volatile { return (countin == countback); }
	
	inline bool isWaiting(void) volatile { 
		if (isFull()) {
			 return true;
		}
		else if (!isSynchronized()) {
			return true;
		}
		else {
			return false;
		}
	}
};
