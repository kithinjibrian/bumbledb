#include "events/ebus.h"
/*
static subscribers_o *subscribers_g = NULL;

subscribers_o *subscribers_get()
{
	if (!subscribers_g)
	{
		subscribers_g = psub_create(20);
		return subscribers_g;
	}

	return subscribers_g;
}

void subscribe(char *topic, fun_generic2_t fun_generic)
{
	subscribers_o *subscribers;
	subscribers = subscribers_get();

	psub_subscribe(topic, fun_generic, subscribers);
}

void publish(char *topic, object_o payload)
{
	subscribers_o *subscribers;
	subscribers = subscribers_get();

	psub_publish(topic, payload, subscribers);
}

void ebus_free()
{
	subscribers_o *subscribers;
	subscribers = subscribers_get();

	psub_free(subscribers);
}*/