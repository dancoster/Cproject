#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 * Encapsulates a bounded minimum queue with variable length maximum size.
 * The elements are BPQueueElement types, which consist a non-negative index which
 * represents the image index to which the element belongs, and a value (type double).
 *
 * The following functions are supported:
 *
 * spBPQueueCreate        	- Creates a new queue
 * spBPQueueCopy			- Creates a new copy of a given queue
 * spBPQueueDestroy 		- Free all resources associated with a queue
 * spBPQueueClear			- Removes all the elements of a given queue
 * spBPQueueSize			- A getter of the current amount of elements in a queue
 * spBPQueueGetMaxSize		- A getter of maximum capacity of a queue
 * spBPQueueEnqueue			- Inserts an element to a queue
 * spBPQueueDequeue			- Removes an element with the lowest value from a queue
 * spBPQueuePeek			- Creates a copy of the element with the lowest value
 * spBPQueuePeekLast		- Creates a copy of the element with the highest value
 * spBPQueueMinValue		- A getter of the minimum value in the queue
 * spBPQueueMaxValue		- A getter of the maximum value in the queue
 * spBPQueueIsEmpty			- Checks if the queue is empty
 * spBPQueueIsFull			- Checks if the queue is full
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;

typedef struct sp_bpq_element_t {
	int index;
	double value;
} BPQueueElement;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new queue in the memory
 * Given a maximum capacity maxSize
 *
 * The maxSize indicates how many elements the queue can hold
 *
 * @return
 * NULL in case allocation failure occurred OR data is NULL OR maxSize <=0
 * Otherwise, the new queue is returned
 */
SPBPQueue* spBPQueueCreate(int maxSize);

/**
 * Allocates a copy of the queue
 *
 * Given the queue source, the functions returns a
 * new queue Q such that:
 * - maxSize(Q) = maxSize(source)
 * - elements(Q) = elements(Q)
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * @return
 * NULL in case memory allocation occurs
 * Others a copy of source is returned.
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * Free all memory allocation associated with queue
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * if queue is NULL nothing happens.
 */
void spBPQueueDestroy(SPBPQueue* source);

/**
 * Removes all the elements in the queue
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * if queue is NULL nothing happens.
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * A getter of the current amount of elements in the queue
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * @return
 * The amount of elements in the queue
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * A getter of maximum capacity of the queue
 *
 * @param source - The source queue
 * @assert (source != NULL)
 * @return
 * The maximum capacity of the queue
 */
int spBPQueueGetMaxSize(SPBPQueue* source);


/**
 * Inserts an element to the queue
 *
 * @param source - The source queue
 * @param index - The inserted element index
 * @param index - The inserted element value
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT message if source==NULL OR value<0 OR index<0
 * SP_BPQUEUE_FULL message if the Queue has already maxsize and the new value is bigger than the biggest value in the queue
 * SP_BPQUEUE_SUCCESS message if the Enqueue succeeded
 * (1)the new element was added to an empty Queue
 * (2)the new value is smaller than the biggest value in the Queue
 * (3)the Queue isn't full
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * removes the element with the lowest value
 *
 * @param source - The source queue
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT message if source==NULL
 * SP_BPQUEUE_EMPTY message if the queue is empty
 * SP_BPQUEUE_SUCCESS message if the Dequeue succeeded
 */

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * Creates a copy of the element with the lowest value in the queue
 *
 * @param source - The source queue
 * @param res - The element to copy the minimum value element to
 * @assert (source != NULL)
 * @assert (res != NULL)
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT message if source==NULL OR res==NULL
 * SP_BPQUEUE_EMPTY message if the queue is empty
 * SP_BPQUEUE_SUCCESS message if the copy succeeded and stored in res
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * Creates a copy of the element with the highest value in the queue
 *
 * @param source - The source queue
 * @param res - The element to copy the maximum value element to
 * @assert (source != NUlL)
 * @assert (res != NUlL)
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT message if source==NULL OR res==NULL
 * SP_BPQUEUE_EMPTY message if the queue is empty
 * SP_BPQUEUE_SUCCESS message if the copy succeeded and stored in res
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);

/**
 * A getter of the minimum value in the queue
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * @assert (source != EMPTY_QUEUE)
 * @return
 * The minimum value in the queue
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * A getter of the maximum value in the queue
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * @assert(!spBPQueueIsEmpty(source))
 * @return
 * The maximum value in the queue
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * Checks if the queue is empty
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * @assert(!spBPQueueIsEmpty(source))
 * @return
 * true if the queue is empty, false otherwise
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * Checks if the queue is full
 *
 * @param source - The source queue
 * @assert (source != NUlL)
 * @return
 * true if the queue is full, false otherwise
 */
bool spBPQueueIsFull(SPBPQueue* source);


#endif
