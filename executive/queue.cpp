#include "queue.h"

/**************************************************************
queue definitions
***************************************************************/

void init_queue(queue *q) {

	q->queue_in = q->queue_out = 0;

}

int add_queue(queue *q, int val) {

	if (q->queue_in == ((q->queue_out - 1 + q->size) % q->size))
	{
		return -1; /* Queue Full*/
	}

	q->queue[q->queue_in] = val;

	q->queue_in = (q->queue_in + 1) % q->size;

	return 0; // No errors
}

int delete_queue(queue *q, int *old)
{
	if (q->queue_in == q->queue_out)
	{
		return -1; /* Queue Empty - nothing to get*/
	}

	*old = q->queue[q->queue_out]; // fetches the old value

	q->queue[q->queue_out] = 0;    // clears the value

	q->queue_out = (q->queue_out + 1) % q->size; // updates the tail counter

	return 0; // No errors
}

char *serialize_queue(char* output_str, queue *q) {

	/*
	* this function can also be program to serialize the queue in array form:
	* References:
	* 1. Parson is a lighweight json library written in C. https://github.com/kgabis/parson
	*/

	int head = q->queue_in;
	int tail = q->queue_out;
	int size = q->size;

	char str[10];

	strcpy(output_str, "[");

	while (tail != head)
	{
		int code = q->queue[tail];

		// print to JSON 
		sprintf(str, "\"G%d\"", code);
		strcat(output_str, str);

		tail = (tail + 1) % size;

		if (tail != head) {
			strcat(output_str, ",");
		}
		else {
			strcat(output_str, "]");
		}
	}
	return output_str;
}
