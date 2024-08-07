#pragma once
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/owner/owner.h"
#include "objects/utils/utils.h"
#include "objects/natives/strings.h"

typedef bool (*fun_list_compare_t)(object_o, object_o);

typedef struct list_node_o
{
	object_o object;
	pthread_mutex_t lock;
	struct list_node_o *next;
	struct list_node_o *prev;
} list_node_o;

list_node_o *new_list_node(object_o object);

string_o *list_join(list_node_o *head, const char *del);

/*
	move forward in linked list
*/
object_o list_next(list_node_o **head);
list_node_o *list_node_next(list_node_o **head);

/*
	move backwards in linked list
*/
object_o list_prev(list_node_o **head);
list_node_o *list_node_prev(list_node_o **head);

/*
	add a node to the linked list head
*/
void list_unshift(list_node_o **head, object_o object);
void list_node_unshift(list_node_o **head, list_node_o *node);

/*
	add a node at the linked list tail
*/
void list_push(list_node_o **head, object_o object);
void list_node_push(list_node_o **head, list_node_o *node);

/*

*/
object_o list_shift(list_node_o **head);
void list_node_shift(list_node_o **head);

/*

*/
object_o list_pop(list_node_o **head);
void list_node_pop(list_node_o **head);

/*
 */
object_o list_find(list_node_o *head, fun_equals_t equal, object_o object);
list_node_o *list_node_find(list_node_o *head, fun_equals_t equal, object_o object);
/*
	delete a node from the linked list
	Both the object and the node are released from memory
*/
void list_node_delete(list_node_o **head, list_node_o *node);
bool list_delete(list_node_o **head, fun_equals_t equal, object_o object);

#endif