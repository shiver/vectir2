#ifndef QUEUE_H
#define QUEUE_H

struct sQueue {
	unsigned int index;
	void *item;
	struct sQueue *next;
};

typedef struct sQueue Queue;

/*
 * Push a new item onto the end of the queue.
 * Returns the index of the newly added item.
 */
unsigned int queue_push(Queue *queue, void *item);

/*
 * Retrieves and removes the first item in the queue.
 * Note: This returns a pointer to the actual item data and not the queue
 * 			structure.
 */
void *queue_pop(Queue *queue, void *item);

/* 
 * Retrieves an item in the queue based on it's index
 * Note: This returns a pointer to the actual item data and not the queue
 * 			structure.
 */
void *queue_get_by_index(Queue *queue, unsigned int index);

/*
 * Frees all resources occupied by the queue.
 * Note: Freeing the queue resources does NOT free the resources the items 
 * 		themselves consume. They will need to be explicitly recovered. 
 *		The ideal time to do this would probably before queue_free() is called.
 */
void queue_free(Queue *queue);

#endif
