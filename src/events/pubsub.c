#include "events/pubsub.h"
/*
subscribers_o *psub_create(int hash_size)
{
	subscribers_o *htable;
	htable = new_htable(hash_size);
	return htable;
}

void psub_subscribe(char *topic, fun_generic2_t fun_generic, subscribers_o *htable)
{
	htable_set(htable, topic, function2(fun_generic));
}

void psub_publish(char *topic, object_o payload, subscribers_o *htable)
{
	list_node_o *node = htable_get_list(htable, MOVE(topic));

	while (node)
	{

		keyvalue_o *kv = (keyvalue_o *)list_get_object(node);

		fun_generic2_t callback = *((fun_generic2_t *)keyvalue_get_value(kv));

		callback(1, share(payload));

		node = list_node_next(&node);
	}

	DROP(topic);
	DROP(payload);
}

void psub_free(subscribers_o *htable)
{
	DROP(htable);
}*/