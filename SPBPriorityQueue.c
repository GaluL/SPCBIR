/*
 * SPBPriorityQueue.c
 *
 *  Created on: May 17, 2016
 *      Author: galkl
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SPBPriorityQueue.h"
#include "SPList.h"

/** Type for defining the Queue**/
struct sp_bp_queue_t {
	int maxSize;
	SPList list;
};

/**
 * Allocates a new  queue in the memory.
 * Given an integer as maximum size
 * The new queue will be empty and will have maximum capacity as given.
 *
 * @return
 * NULL in case allocation failure occurred OR maxSize <=0
 * Otherwise, the new queue is returned
 */
SPBPQueue spBPQueueCreate(int maxSize)
{
	SPBPQueue queue = NULL;
	// checking for basic condition
	if (maxSize <= 0)
	{
		return NULL;
	}

	// allocating memory for the new queue
	queue = (SPBPQueue)malloc(sizeof(*queue));
	if (!queue)
	{
		return NULL;
	}

	// establishing the queue fields
	queue->maxSize = maxSize;
	queue->list = spListCreate();
	if (!queue->list)
	{
		free(queue);
		return NULL;
	}

	return queue;
}

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
 * @return
 * NULL in case memory allocation occurs
 * Others a copy of source is returned.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source)
{
	SPBPQueue copy = NULL;

	// checking for basic conditions
	assert(source);
	assert(source->list);
	// allocating new memory for the copy queue
	copy = spBPQueueCreate(source->maxSize);
	//	if allocation problem occurred return null
	if (!copy)
	{
		return NULL;
	}
	// free the empty list that was created for copy
	spListDestroy(copy->list);
	copy->list = spListCopy(source->list);
	//	if allocation problem occurred return null
	if (!copy->list)
	{
		free(copy);
		return NULL;
	}


	return copy;
}

/**
 * Free all memory allocation associated with queue,
 *
 * @param source - The source queue
 *
 * if queue is NULL nothing happens.
 */
void spBPQueueDestroy(SPBPQueue source)
{
	if (!source)
	{
		return;
	}

	if (source->list)
	{
		spListDestroy(source->list);
	}

	free(source);
	source = NULL;
}

/**
 * removes all the elements in the queue,
 *
 * @param source - The source queue
 *
 */
void spBPQueueClear(SPBPQueue source)
{
	// clearing the queues list
	if (source && source->list)
	{
		spListClear(source->list);
	}
}

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
int spBPQueueSize(SPBPQueue source)
{
	// checking for basic conditions
	if (!source)
	{
		return -1;
	}
	// if list was not initiated
	if (!source->list)
	{
		return 0;
	}

	return spListGetSize(source->list);
}

/**
 * returns the maximum capacity of the queue
 *
 * @param source - The source queue
 *
 * @return
 * Integer represent the maximum capacity of the queue  -----------  -1 in case of null?
 */
int spBPQueueGetMaxSize(SPBPQueue source)
{
	if (!source)
	{
		return -1;
	}

	return source->maxSize;
}

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
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element)
{
	SPListElement element_copy = NULL;
	SPListElement list_iterator = NULL;
	bool should_dequeue = false;
	bool element_inserted = false;
	int i = 0;

	// checking for basic conditions
	if (!element || !source || !source->list)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	//	creating the copy of the element to be inserted
	element_copy = spListElementCopy(element);
	//	checking for allocation problem
	if (!element_copy)
	{
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}

	//checking for the the first element in the queue
	list_iterator = spListGetFirst(source->list);
	//	if null means the queue is empty and we will insert him first
	if (!list_iterator)
	{
		//	since queue is empty we insert the element to the first place in the queue, free used memory and print success
		if (spListInsertFirst(source->list, element_copy) == SP_LIST_OUT_OF_MEMORY)
		{
			spListElementDestroy(element_copy);
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}

		spListElementDestroy(element_copy);

		return SP_BPQUEUE_SUCCESS;
	}
	// if true  means that the queue is full and  in the end of the loop we may need to dequeue
	should_dequeue = spListGetSize(source->list) == (source->maxSize);
	// we will run the loop until we got to the maxsize or we already inserted the element
	while (i < source->maxSize && !element_inserted && list_iterator)
	{
		//	looking for a place for the element in the queue - if smaller the our current enter before
		if (spListElementGetValue(list_iterator) > spListElementGetValue(element_copy))
		{
			//	insert before the current if failed free used memory and return memory allocation problem massage
			if (spListInsertBeforeCurrent(source->list, element_copy)
					== SP_LIST_OUT_OF_MEMORY)
			{
				spListElementDestroy(element_copy);
				return SP_BPQUEUE_OUT_OF_MEMORY;
			}

			element_inserted = true;
		}
		// if nothing inserted check for the next in the queue
		if (!element_inserted)
		{
			list_iterator = spListGetNext(source->list);
		}

		i++;
	}
	// if the element have not inserted yet and the queue is not full means the element value is the largest and need to be enter last in the queue
	if (!element_inserted && !should_dequeue)
	{
		if (spListInsertLast(source->list, element_copy) == SP_LIST_OUT_OF_MEMORY)
		{
			spListElementDestroy(element_copy);
			return SP_BPQUEUE_OUT_OF_MEMORY;
		}

		element_inserted = true;
	}
	// if the element was inserted and the queue was full means we need to dequeue the last element the now in the queue
	if (should_dequeue && element_inserted)
	{
		for (; i < source->maxSize; i++)
		{
			list_iterator = spListGetNext(source->list);
		}

		spListRemoveCurrent(source->list);
	}

	//TODO: understand if should free element or not, since list insert already make a new copy (double allocation)
	spListElementDestroy(element_copy);
	return SP_BPQUEUE_SUCCESS;
}

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
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source)
{
	// checking for basic conditions
	if (!source  || !source->list)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	if (spBPQueueIsEmpty(source))
	{
		return SP_BPQUEUE_EMPTY;
	}

	// removing the first element since he is the one with lowest value
	spListGetFirst(source->list);
	spListRemoveCurrent(source->list);

	return SP_BPQUEUE_SUCCESS;
}

/**
 * Allocates a copy of the element with the lowest value
 *
 * @param source - the source queue
 *
 *
 * @return
 * NULL in case source is NULL OR source->list is NULL OR the queue is empty
 * A new copy of the element with  the minimum value in the queue
 */
SPListElement spBPQueuePeek(SPBPQueue source)
{
	SPListElement head = NULL;

	if (!source || !source->list || !(spListGetSize(source->list) > 0))
	{
		return NULL;
	}

	// getting the first element since it got the lowest value
	head = spListGetFirst(source->list);

	return spListElementCopy(head);
}

/**
 * Allocates a copy of the element with the highest value
 *
 * @param source - the source queue
 *
 *
 * @return
 * NULL in case source is NULL OR source->list is NULL OR the queue is empty
 * a new copy of the element with  the maximum value in the queue
 */
SPListElement spBPQueuePeekLast(SPBPQueue source)
{
	SPListElement iterator = NULL;
	int i = 0;

	if (!source || !source->list || !(spListGetSize(source->list) > 0))
	{
		return NULL;
	}

	iterator = spListGetFirst(source->list);

	// getting to the end of the queue
	for (i = 1; i < spListGetSize(source->list); i++)
	{
		iterator = spListGetNext(source->list);
	}

	// return a copy of the last element in the queue since it got the highest value
	return spListElementCopy(iterator);
}

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
double spBPQueueMinValue(SPBPQueue source)
{
	SPListElement head = NULL;
	// checking for basic conditions
	assert(source);
	assert(source->list);

	// getting the first element since it got the lowest value
	head = spListGetFirst(source->list);

	return spListElementGetValue(head);
}

/**
 * returns an integer represent the maximum value in the queue
 *
 * @param source - the source queue
 *
 * @assert (source != NULL)
 * @assert (source->list != NULL)
 *
 *
 * @return
 * Integer represent the maximum value in the queue
 */
double spBPQueueMaxValue(SPBPQueue source)
{
	SPListElement iterator = NULL;
	int i = 0;
	// checking for basic conditions
	assert(source);
	assert(source->list);

	iterator = spListGetFirst(source->list);
	// getting to the end of the queue
	for (i = 1; i < spListGetSize(source->list); i++)
	{
		iterator = spListGetNext(source->list);
	}

	// return the value of the last element in the queue since it got the highest value
	return spListElementGetValue(iterator);
}

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
bool spBPQueueIsEmpty(SPBPQueue source)
{
	assert(source);
	assert(source->list);
	// check if list size of the queue is == 0 or list isn't initialized yet
	return spListGetSize(source->list) == 0 ? true : false;
}

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
bool spBPQueueIsFull(SPBPQueue source)
{
	assert(source);

	if (!source->list)
	{
		return false;
	}

	// check if list size of the queue is == maxSize
	return spListGetSize(source->list) == source->maxSize ? true : false;
}

