#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "../log/log.h"

/*
 * Returns the Queue struct for the last item in the queue.
 * before - points to the Queue struct directly before the last or NULL if
 *			the last item is also the first.
 */
static
Queue *
get_last(Queue *queue, Queue *before) {
	Queue *curr;
	
	if (queue == NULL) {
		return NULL;
	}
	
	curr = queue;
	while (curr->next != NULL) {
		before = curr;
		curr = curr->next;
	}
	
	return curr;
}

unsigned int 
queue_push(Queue *queue, void *item) {
	Queue *last;
	Queue *new;
	
	// find the end of the queue and add the item there
	last = get_last(queue);
	
	new = malloc(sizeof(Queue));
	if (new == NULL) {
		LOG_ERROR("Could not add item to queue. Insufficient memory.");
		return -1;
	}
	memset(new, '\0', sizeof(Queue));
	
	new->item = item;
	
	// determine whether the new item falls at the first position
	// also sets the index (zero based)
	if (last == NULL) {
		new->index = 0;
		queue = new;
	} else {
		new->index = ++last->index;
		last->next = new;
	}
		
	return new->index;
}

void * 
queue_pop(Queue *queue) {
	Queue *last;
	Queue *before;
	void *item;

	if (queue == NULL) {
		LOG_ERROR("Could not pop item from queue. Queue has not been " \
			"initiliased");
		return NULL;
	}
	
	// get last item in the queue and keep the item data
	last = get_last(queue, before);
	item = last->item;
	
	if (before != NULL) {
		// remove references to the last item in the queue
		before->next = NULL;
	}
	free(last);
	
	return item;
}

void *
queue_get_by_index(Queue *queue, unsigned int index) {
	Queue *curr;
	
	if (queue == NULL) {
		LOG_ERROR("Could not get item in queue. Queue has not been " \
			"initiliased.");
		return NULL;
	}
	
	if (queue->index == index) {
		return queue->item;
	}
	
	curr = queue;
	while (curr->next != NULL) {
		if (curr->index == index) {
			return curr->item;
		}
		curr = curr->next;
	}
	
	return NULL;
}

void 
queue_free(Queue *queue) {
	Queue *curr;
	Queue *prev;
	
	if (queue == NULL) {
		LOG_ERROR("Could not free queue resources. Queue has not been " \
			"initialised.");
		return;
	}
	
	// free resources for the entire queue
	// we are only interested in the queue structures and not the data they hold
	curr = queue;
	while (curr->next != NULL) {
		prev = curr;
		curr = curr->next;
		
		free(prev);
	}
	
	free(queue);
}
