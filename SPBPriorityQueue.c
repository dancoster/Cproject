#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define EMPTY_QUEUE_INDEX -1

struct sp_bp_queue_t {
	BPQueueElement* elements;	//pointer to elements array
	int maxSize;				//max size of the queue
	int size;					//current amount of elements on the queue
	int start; 					//Indicates the start of the queue
};

SPBPQueue* spBPQueueCreate(int maxSize) {
	if (maxSize<=0)
		return NULL;

	SPBPQueue *res = (SPBPQueue*) malloc(sizeof(SPBPQueue));
	if (res == NULL)				//memory allocation failure
		return NULL;

	res->elements = (BPQueueElement*) malloc(maxSize*sizeof(BPQueueElement));
	if (res->elements == NULL) {	//memory allocation failure
		free(res);
		return NULL;
	}

	res->maxSize = maxSize;
	res->size = 0;
	res->start = EMPTY_QUEUE_INDEX;

	return res;
}

SPBPQueue* spBPQueueCopy(SPBPQueue* source) {
	assert(source!=NULL);

	SPBPQueue *copy = spBPQueueCreate(source->maxSize);

	memcpy(copy->elements, source->elements, (source->maxSize)*sizeof(BPQueueElement)); //copy source elements to new copy
	copy->maxSize = source->maxSize;
	copy->size = source->size;
	copy->start = source->start;

	return copy;
}

void spBPQueueDestroy(SPBPQueue* source) {
	if (source == NULL)
		return;

	free(source->elements);  //free array memory
	free(source);			 //free queue memory
	return;
}

void spBPQueueClear(SPBPQueue* source) {
	if (source == NULL)
		return;

	source->size = 0;
	source->start = EMPTY_QUEUE_INDEX;
}

int spBPQueueSize(SPBPQueue* source) {
	assert(source!=NULL);

	return source->size;
}

int spBPQueueGetMaxSize(SPBPQueue* source) {
	assert(source!=NULL);

	return source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value) {
	if (index<0 || value<0 || source==NULL)					//invalid arguments
		return SP_BPQUEUE_INVALID_ARGUMENT;

	if (spBPQueueIsEmpty(source)) { 		//queue is empty
		source->elements[0].index = index;	//setting first element index
		source->elements[0].value = value;	//setting first element value
		source->start = 0;					//updating queue start
		source->size++;						//updating queue size
		return SP_BPQUEUE_SUCCESS;			//msg
	}

	BPQueueElement *temp = (BPQueueElement*) malloc(sizeof(BPQueueElement));
	if (temp==NULL) //memory allocation failure
		return SP_BPQUEUE_OUT_OF_MEMORY;

	spBPQueuePeekLast(source, temp);
	if (value > temp->value) { 					//last element value < value
		if (spBPQueueIsFull(source)) {			//queue is full
			free(temp);
			return SP_BPQUEUE_FULL;				//msg
		}
		else {									//queue isn't full, adding to end of queue
			source->elements[(source->start+source->size)%source->maxSize].index = index;
			source->elements[(source->start+source->size)%source->maxSize].value = value;
			source->size++;						//updating queue size
			free(temp);
			return SP_BPQUEUE_SUCCESS;			//msg
		}
	}
	free(temp);

	int temp_index = index;
	int temp2_index;
	int current_index;
	double temp_value = value;
	double temp2_value;
	double current_value;
	int i;
	for (i=0; i<(source->size); i++) {  	//iterating over the queue elements
		current_index = source->elements[(i+source->start)%source->maxSize].index; //current_index=elements[i].index
		current_value = source->elements[(i+source->start)%source->maxSize].value; //current_index=elements[i].value

		//finding the first element which (element[i].value>value) || (element[i].value==value && element[i].index>index)
		//then switching element[i] with value/index
		//continue switching element[i+1] with element[i] until i<queue.size
		if ((temp_value<current_value) || ((temp_value==current_value) && (temp_index<current_index))) {
			temp2_index = current_index;
			temp2_value	= current_value;
			source->elements[(i+source->start)%source->maxSize].index = temp_index;
			source->elements[(i+source->start)%source->maxSize].value = temp_value;
			temp_index = temp2_index;
			temp_value = temp2_value;
		}
	}
	//if queue isn't full, then add last element from the loop above to end of queue
	if (!spBPQueueIsFull(source)) {
		source->elements[(i+source->start)%source->maxSize].index = current_index;
		source->elements[(i+source->start)%source->maxSize].value = current_value;
		source->size++; 				//updating queue size
	}
	return SP_BPQUEUE_SUCCESS; //msg
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source) {
	if (source==NULL)							//invalid argument
		return SP_BPQUEUE_INVALID_ARGUMENT;
	if (spBPQueueIsEmpty(source))  //queue is empty
		return SP_BPQUEUE_EMPTY;

	//No need to actually free elements, only increment index.
	source->size--;
	if(source->size==0)  //if last element was deleted
		source->start = EMPTY_QUEUE_INDEX;
	else
		source->start = (source->start+1)%source->maxSize;

	return SP_BPQUEUE_SUCCESS;//msg
}

SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res) {
	if (res == NULL || source==NULL)			//invalid arguments
		return SP_BPQUEUE_INVALID_ARGUMENT;

	if (spBPQueueIsEmpty(source))  //queue is empty
		return SP_BPQUEUE_EMPTY;   //msg

	//extracting first element
	res->index = source->elements[source->start].index;
	res->value = source->elements[source->start].value;
	return SP_BPQUEUE_SUCCESS;	   //msg
}

SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res) {
	if (res == NULL || source==NULL)			//invalid arguments
		return SP_BPQUEUE_INVALID_ARGUMENT;

	if (spBPQueueIsEmpty(source))  //queue is empty
		return SP_BPQUEUE_EMPTY;   //msg

	//extracting last element
	res->index = source->elements[(source->start+source->size-1)%source->maxSize].index;
	res->value = source->elements[(source->start+source->size-1)%source->maxSize].value;
	return SP_BPQUEUE_SUCCESS;     //msg
}

double spBPQueueMinValue(SPBPQueue* source) {
	assert(source!=NULL);
	assert(!spBPQueueIsEmpty(source));  		//queue is empty

	return source->elements[source->start].value;
}

double spBPQueueMaxValue(SPBPQueue* source) {
	assert(source!=NULL);
	assert(!spBPQueueIsEmpty(source));			//queue is empty

	return source->elements[(source->start+source->size-1)%source->maxSize].value;
}

bool spBPQueueIsEmpty(SPBPQueue* source) {
	assert(source!=NULL);

	if (source->start==EMPTY_QUEUE_INDEX)
		return true;
	else
		return false;
}

bool spBPQueueIsFull(SPBPQueue* source) {
	assert(source!=NULL);

	if (source->size == source->maxSize)
		return true;
	else
		return false;
}
