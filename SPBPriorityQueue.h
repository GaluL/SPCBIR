#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new  queue in the memory.
 * Given an integer as maximum size
 * The new queue will be empty and will have maximum capacity as given.
 *
 * @return
 * NULL in case allocation failure occurred OR maxSize <=0
 * Otherwise, the new queue is returned
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Allocates a copy of the given queue.
 * Given the source queue, the functions returns a
 * new queue Q such that:
 * - Q_i = source_i (The ith element of source and P are the same)
 * - maxSize(Q) =maxSize(source) (Q and source have the same maxSize)
 *
 * @param source - The source queue
 *
 * @assert (source != NULL)
 * @assert (source->list != NULL)
 *
 * @return
 * NULL in case memory allocation occurs
 * Others a copy of source is returned.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Free all memory allocation associated with queue,
 *
 * @param source - The source queue
 *
 * If queue is NULL nothing happens.
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * removes all the elements in the queue,
 *
 * @param source - The source queue
 *
 * @assert (source != NULL)
 * @assert (source->list != NULL)
 */
void spBPQueueClear(SPBPQueue source);


/**
 * returns the number of elements in the queue
 *
 * @param source - The source queue
 *
 *
 * @return
 *  0 if list was not initiated
 * -1 in case source is NULL
 * Integer represent the number of elements in the queue
 */
int spBPQueueSize(SPBPQueue source);

/**
 * returns the maximum capacity of the queue
 *
 * @param source - The source queue
 *
 * @return
 * -1 in case source is NULL
 * Integer represent the maximum capacity of the queue
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Allocates a copy of a given an element.
 * The element will be added to the queue such that:
 * The queue will remain in Ascending order sorted by the element value
 * if the queue is at full capacity, the element with the highest priority value is ejected from the queue
 *
 * @param source - the queue for which to add an element
 * @param element - the element to insert. A copy of the element will be inserted
 *
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT in case of element is NULL OR source is NULL OR source->list is NULL
 * SP_BPQUEUE_OUT_OF_MEMORY in case allocation failure occurred
 * Otherwise, SP_BPQUEUE_SUCCESS -the element has been inserted successfully
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * removes the element with the lowest value
 *
 * @param source - the queue for which to remove an element
 *
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT in case source is NULL OR source->list is NULL
 * SP_BPQUEUE_EMPTY in case the queue is empty
 * Otherwise, SP_BPQUEUE_SUCCESS -the element with the lowest value has been dequeued successfully
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Allocates a copy of the element with the lowest value
 *
 * @param source - the source queue
 *
 * @return
 * NULL in case source is NULL OR source->list is NULL OR the queue is empty
 * A new copy of the element with  the minimum value in the queue
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Allocates a copy of the element with the highest value
 *
 * @param source - the source queue
 *
 * @assert (source != NULL)
 * @assert (source->list != NULL)
 * @assert (the queue is not empty)
 *
 * @return
 * a new copy of the element with  the maximum value in the queue
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * returns an double represent the minimum value in the queue
 *
 * @param source - the source queue
 *
 * @assert (source != NULL)
 * @assert (source->list != NULL)
 *
 *
 * @return
 * Double represent the minimum value in the queue
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * returns a double represent the maximum value in the queue
 *
 * @param source - the source queue
 *
 * @assert (source != NULL)
 * @assert (source->list != NULL)
 *
 *
 * @return
 * Double represent the maximum value in the queue
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * returns true if the queue is empty
 *
 * @param source - the source queue
 *
 * @assert (source != NULL)
 * @assert (source->list != NULL)
 *
 *
 * @return
 * True if queue is empty otherwise false
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * returns true if the queue is at full capacity
 *
 * @param source - the source queue
 *
 * @assert (source != NULL)
 *
 * @return
 * True if the queue is at full capacity otherwise false
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
