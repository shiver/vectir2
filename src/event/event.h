#ifndef EVENT_H
#define EVENT_H

typedef (*ptrEventCallback)(unsigned int size, char *data);

struct sSubscriber {
	unsigned int id;
	unsigned int event_id;
	ptrEventCallback callback;
};

typedef struct sSubscriber Subscriber;

struct sEvent {
	unsigned int id;
	void *data;
};

typedef struct sEvent Event;

/* 
 * Initialises resources required for event processing.
 * Note: event_close() MUST be explicitly called to free the resources once
 * you are done using the event handler.
 */ 
void event_init();

/*
 * Free resources associated with event handling.
 */
void event_close();

/*
 * Checks if there are any events waiting on the queue and makes calls to the
 * correct places based on their types. This is not a blocking call and will
 * return even if no events were processed.
 * Returns an unsigned int which holds the number of events that were
 * processed in this call.
 */
unsigned int event_process();

/*
 * The caller subscribes to a particular event by supplying an event ID and 
 * corresponding callback method function pointer. When an event with the
 * specified ID is encountered event_process(), the callback method
 * will be called.
 * Returns an unsigned int which is the unique handle of event ID and callback
 * combination.
 */
unsigned int event_subscribe(unsigned int event_id, ptrEventCallback callback);

/*
 * 
 */
void event_trigger(unsigned int event_id);


#endif