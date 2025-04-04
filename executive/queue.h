#pragma once

#include <iostream>

# ifndef NUM_MODALS
# define NUM_MODALS     14 
# endif

/* queue */
typedef struct
{
	int size = NUM_MODALS;     // the size of the queue
	int queue[NUM_MODALS];	   // define memory space for the queue
	int queue_in;			   // an index for the in element in the queue
	int queue_out;			   // an index for the out element in the queue

} queue;

/**************************************************************
queue prototypes
***************************************************************/
void init_queue(queue *q);
int  add_queue(queue *q, int val);
int  delete_queue(queue *q, int *old);
char *serialize_queue(char* output_str, queue *q);