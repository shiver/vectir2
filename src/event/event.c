#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"
#include "../util/log/log.h"

#define MAX_SUBSCRIBERS 	1024
#define MAX_EVENTS 			1024

static unsigned int last_subscriber_id = 0;
static Queue *event_subscribers;
static unsigned int num_events;
static Queue *event_queue;

void
event_init() {
	LOG_DEBUG("Initiliasing event handling...");
	num_events = 0;
	event_subscribers = NULL;
	event_queue = NULL;
}

/* 
 * Returns the number of events currently sitting in the queue.
 */
static
unsigned int
peek_events() {
	return num_events;
}

/*
 * Places the specified event onto the event_queue
 * Note: The queue operates on a FIFO basis.
 */
static
void
push_event(Event *event) {
	Event *cur_event;
	
	if (event == NULL) {
		LOG_ERROR("Attempted to push a NULL event onto the queue. Ignoring...");
	}
	
	LOG_DEBUG("Adding event to queue...");
	
	if (event_queue == NULL) {
		// make this the first event in the queue
		event_queue = event;
	} else {
		// add to the end of the queue
		cur_event = event_queue;
		while (cur_event->next != NULL) {
			cur_event = cur_event->next;
		}
		
		cur_event->next = event;
	}
	
	num_events++;
	
	LOG_DEBUG("Event added. %d events on the queue.", peek_events());
}

/*
 * Retrieves and removes the first event from the queue
 */
static
Event *
pop_event() {
	Event *event;
	Event *next_event;
	
	if (event_queue == NULL) {
		LOG_ERROR("Could not retrieve event. Event queue is empty.");
		return NULL;
	}
	
	event = event_queue;
	next_event = event_queue->next;
	event_queue = next_event;
	num_events--;
	
	if (num_events < 0) {
		LOG_ERROR("Event queue is reporting a negative number of events. " \
			"Something is very wrong.");
	}
	
	return event;
}

/*
 * Retrives a list of all subscribers for the specified event ID
 */
static
Subscriber *
get_subscribers(unsigned int event_id) {
	Subscriber *subscribers = NULL;
	Subscriber *cur_event_sub;
	Subscriber *last_dst_sub;
	
	if (event_subscribers == NULL) {
		return NULL;
	}
	
	if (event_subscribers->event_id == event_id) {
		last_dst_sub = malloc(sizeof(Subscriber));
		memset(last_dst_sub, '\0', sizeof(Subscriber));
		copy_subscriber(last_dst_sub, event_subscriber);
		subscribers = last_dst_sub;
	}
	
	cur_event_sub = event_subscribers;
	while(cur_event_sub->next != NULL) {
		if (cur_event_sub->event_id == event_id) {
			if (subscribers == NULL) {
				// handle the first subscriber found
				subscribers = malloc(sizeof(Subscriber));
				memset(subscribers, '\0', sizeof(Subscriber));
				copy_subscriber(subscribers, cur_event_sub);
				last_dst_sub = subscribers;
			} else {
				// handle subsequent subscribers
				last_dst_sub->next = malloc(sizeof(Subscriber));
				memset(last_dst_sub->next, '\0', sizeof(Subscriber));
				copy_subscriber(last_dst_sub->next, cur_event_sub);
				last_dst_sub = last_dst_sub->next;
			}
		}
		cur_event_sub = cur_event_sub->next;
	}
	
	return subscribers;
}

static
void
copy_subscriber(Subscriber *dest, Subscriber *src) {
	subscribers->id = event_subscribers->id;
	subscribers->event_id = event_subscribers->event_id;
	subscribers->callback = malloc(sizeof(ptrEventCallback));
	memcpy(subscribers->callback, event_subscribers->callback, 
		sizeof(ptrEventCallback));
}

static
Subscriber *
get_last_subscriber() {
	
}

void
event_close() {

}

unsigned int
event_subscribe(unsigned int event_id, ptrEventCallback callback) {
	Subscriber *subscriber;
	Subscriber *cur_sub;
	
	LOG_DEBUG("Adding new subscriber to event ID %d...", event_id);
	
	subscriber = malloc(sizeof(Subscriber));
	if (subscriber == NULL) {
		LOG_SEVERE("Could not assign new subscriber for event %d. In " \
			"sufficient memory.", event_id);
		return;
	}
	memset(subscriber, '\0', sizeof(Subscriber));
	
	subscriber->id = ++last_subscriber_id;
	subscriber->event_id = event_id;
	
	subscriber->callback = malloc(sizeof(ptrEventCallback));
	if (subscriber->callback == NULL) {
		LOG_SEVERE("Could not assign new subscriber for event %d. In " \
			"sufficient memory.", event_id);
		free(subscriber);
		return;
	}
	memcpy(subscriber->callback, callback, sizeof(ptrEventCallback));
		
	// add the subscriber to the list
	queue_push(event_subscribers, subscriber);
		
	LOG_DEBUG("Subscriber added %d", subscriber->id);
	return subscriber->id;
}

void
event_trigger(unsigned int event_id, void *data) {
	Event *event;
	
	LOG_DEBUG("Triggering event with ID %d...", event_id);
	// allocate resources for event and push it onto the back of the event queue
	event = malloc(sizeof(Event));
	if (event == NULL) {
		LOG_ERROR("Could not trigger event. Insufficient memory.");
		return;
	}
	memset(event, '\0', sizeof(Event));
	
	event->id = id;
	event->data = data;
	
	if (queue_push(event_queue, event) > -1) {
		// event pushed successfully
		num_events++;
	}
}

unsigned int
event_process() {
	Event *event;
	Subscribers *subscribers;
	int events_processed = 0;
	
	// deal with all events currently on the queue
	// TODO: may want to consider sticking a threshold on the number of
	//		events we handle in each batch if there are noticable performance
	//		issues.
	while (peek_events() > 0) {
		
		event = (Event *)pop_event();
		
		subscribers = get_subscribers(event->id);
		if (subscribers != NULL) {
			LOG_DEBUG("Subscribers found to handle event ID %d", event->id);
		}
		
		// The event should now be at the end of it's lifecycle and as such,
		// it's resources can be freed
		free(event);
		events_processed++;
	}
	
	return events_processed;
}
