#include <iostream>

#include "buffer.h"

/**************************************************************
Buffer object
***************************************************************/

int Buffer::write(char* block) volatile
{
	if (countin == ((countout - 1 + size) % size))
	{
		return -1; /* Queue Full*/
	}

	const char* src = block;

	int src_length = lenght;

	char *dst = (char*) buffer[countin];

	strcpy_s(dst, src_length, src);

	// updates the head counter of the buffer
	countin = (countin + 1) % size;

	// and the number of element in the buffer
	n_elem++;

	return 0;
}

char* Buffer::read() volatile
{
	if (countin == countout)
	{
		return NULL; /* Queue Empty - nothing to get*/
	}

	// load the string pointed by char
	char* block_pnt = (char*) buffer[countout];

	// updates the tail counter
	countout = (countout + 1) % size;

	// and the number of element in the buffer
	n_elem--;

	return block_pnt; // No errors

					  /*
					  * alternatively:
					  *
					  * int deleteBuffer(char** block_pnt) {
					  *
					  * *block_pnt =  buffer[countout];  // block is a char* type. the location in memory of the pointer is defined outside
					  *									 // the function and it cannot be globally modified by the it. However, the value of
					  *									 // a block can be globally modified by the function by passing it a pointer to it
					  *									 // (char** block_pnt)
					  *
					  */
}
