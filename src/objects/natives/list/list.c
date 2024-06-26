#include "objects/print/print.h"
#include "objects/natives/list/list.h"

struct list_node_o
{
	object_o object;
	pthread_mutex_t lock;
	struct list_node_o *next;
	struct list_node_o *prev;
};

void list_visitor(object_o object, fun_process_t fun_process)
{
	list_node_o *node = (list_node_o *)object;

	post_visit(node->object, fun_process);

	if (node->next)
	{
		post_visit(node->next, fun_process);
	}
}

object_o list_str(object_o object)
{
	list_node_o *node = (list_node_o *)object;

	SMART string_o *cnt = string_from("");
	while (node)
	{
		if (object_typeof(node->object) == NT_STRING_O)
		{
			cnt = string_push(MOVE(cnt), string_format("\"%q\"", node->object));
		}
		else
		{
			cnt = string_push(MOVE(cnt), str(node->object));
		}

		if (node->next)
		{
			cnt = string_push(MOVE(cnt), string_from(", "));
		}
		node = node->next;
	}

	return string_format("(%q)", cnt);
}

object_o list_get_state(object_o object)
{
	list_node_o *node = (list_node_o *)object;
	array_o *state = new_array(0);

	while (node)
	{
		array_push(state, share(node->object));
		node = node->next;
	}

	return state;
}

list_node_o *new_list_node(object_o object)
{
	list_node_o *node;

	node = (list_node_o *)alloc(1, sizeof(list_node_o), NT_LIST_NODE_O);

	node->object = object;
	node->next = NULL;
	node->prev = NULL;

	static const vtable_t vt = {
		.__str__ = list_str,
		.__visitor__ = list_visitor,
		.__get_state__ = list_get_state};

	object_reg_dunders(node, &vt);

	return node;
}

object_o list_get_object(list_node_o *node)
{
	return node->object;
}

object_o list_get_next(list_node_o *node)
{
	return node->next;
}

void list_set_object(list_node_o *node, object_o object)
{
	node->object = object;
}

void list_set_next(list_node_o **node, list_node_o *node_next)
{
	(*node)->next = node_next;
}

string_o *list_join(list_node_o *head, const char *del)
{
	string_o *cnt = string_from("");
	while (head)
	{
		cnt = string_push(MOVE(cnt), share(head->object));
		if (head->next)
		{
			cnt = string_push(MOVE(cnt), string_from("%s", del));
		}
		head = head->next;
	}

	return cnt;
}

/*
	Add an object to the head of the linked list
*/
void list_unshift(list_node_o **head, object_o object)
{
	list_node_o *new_node = new_list_node(object);

	if (!*head)
	{
		*head = new_node;
		return;
	}

	new_node->next = *head;
	(*head)->prev = new_node;
	*head = new_node;
}

/*
	Add a node to the head of the linked list
*/
void list_node_unshift(list_node_o **head, list_node_o *node)
{
	if (!*head)
	{
		*head = node;
		return;
	}

	pthread_mutex_lock(&((*head)->lock));

	node->next = *head;
	(*head)->prev = node;
	*head = node;

	pthread_mutex_unlock(&((*head)->lock));
}

/*
	Add an object linked list tail
*/
void list_push(list_node_o **head, object_o object)
{
	list_node_o *new_node, *temp;

	new_node = new_list_node(object);

	if (!*head)
	{
		*head = new_node;
		return;
	}

	temp = *head;

	/*
		iterate to the last node
	*/
	while (temp->next)
	{
		temp = temp->next;
	}

	pthread_mutex_lock(&((*head)->lock));

	temp->next = new_node;
	new_node->prev = temp;

	pthread_mutex_unlock(&((*head)->lock));
}

/*
	Add a node linked list tail
*/
void list_node_push(list_node_o **head, list_node_o *node)
{
	list_node_o *temp;

	if (!*head)
	{
		*head = node;
		return;
	}

	temp = *head;

	/*
		iterate to the last node
	*/
	while (temp->next)
	{
		temp = temp->next;
	}

	pthread_mutex_lock(&((*head)->lock));

	temp->next = node;
	node->prev = temp;

	pthread_mutex_unlock(&((*head)->lock));
}

object_o list_shift(list_node_o **head)
{
	object_o object;
	list_node_o *temp;

	if (*head == NULL)
	{
		return NULL;
	}

	temp = *head;
	object = temp->object;

	*head = (*head)->next;

	if (*head != NULL)
	{
		(*head)->prev = NULL;
	}

	/*
		Skip the object from dropping
	*/
	temp->object = NULL;

	DROP(temp);

	return object;
}

void list_node_shift(list_node_o **head)
{
	if (*head == NULL)
	{
		return;
	}

	*head = (*head)->next;

	if (*head != NULL)
	{
		(*head)->prev = NULL;
	}
}

object_o list_pop(list_node_o **head)
{
	SMART object_o object = NULL;
	SMART list_node_o *temp = NULL;

	if (*head == NULL)
	{
		printf("Linked list is empty. Cannot remove last element.\n");
		return NULL;
	}

	temp = *head;

	/*
		iterate to last node
	*/
	while (temp->next)
	{
		temp = temp->next;
	}

	object = temp->object;
	if (temp->prev != NULL)
	{
		temp->prev->next = NULL;
	}
	else
	{
		*head = NULL;
	}

	/*
		Stop the linked list from deallocating the object
	*/
	temp->object = NULL;

	RETURN(object);
}

void list_node_pop(list_node_o **head)
{
	SMART list_node_o *temp = NULL;

	if (*head == NULL)
	{
		printf("Linked list is empty. Cannot remove last element.\n");
		return;
	}

	temp = *head;

	/*
		iterate to last node
	*/
	while (temp->next)
	{
		temp = temp->next;
	}

	if (temp->prev != NULL)
	{
		temp->prev->next = NULL;
	}
	else
	{
		*head = NULL;
	}
}

object_o list_find(list_node_o *head, fun_equals_t equal, object_o object)
{
	list_node_o *current = head;
	SMART object_o smt_object = object;

	while (current)
	{
		if (equal(current->object, smt_object))
		{
			return current->object;
		}
		current = current->next;
	}

	return NULL;
}

list_node_o *list_node_find(list_node_o *head, fun_equals_t equal, object_o object)
{
	list_node_o *current;
	SMART object_o smt_object = object;

	current = head;
	while (current)
	{
		if (equal(current->object, smt_object))
		{
			return current;
		}
		current = current->next;
	}

	return NULL;
}

bool list_delete(list_node_o **head, fun_equals_t equal, object_o object)
{
	list_node_o *node = NULL;
	object_o smt_object = object;

	node = list_node_find(*head, equal, MOVE(smt_object));

	if (node)
	{
		list_node_delete(head, node);
		return true;
	}

	return false;
}

void list_node_delete(list_node_o **head, list_node_o *node)
{
	if (*head == NULL || node == NULL)
	{
		return;
	}

	SMART list_node_o *smt_node = node;

	if (smt_node == *head)
	{
		*head = smt_node->next;
		if (*head != NULL)
		{
			(*head)->prev = NULL;
		}
	}

	if (smt_node->prev != NULL)
	{
		smt_node->prev->next = smt_node->next;
	}

	if (smt_node->next != NULL)
	{
		smt_node->next->prev = smt_node->prev;
	}

	smt_node->next = NULL;
}

object_o list_next(list_node_o **head)
{
	if (*head == NULL)
		return NULL;

	object_o object = (*head)->object;

	*head = (*head)->next;

	return object;
}

list_node_o *list_node_next(list_node_o **head)
{
	if (*head == NULL)
		return NULL;

	*head = (*head)->next;

	return *head;
}

object_o list_prev(list_node_o **head)
{
	if (*head == NULL)
		return NULL;

	object_o object = (*head)->object;

	*head = (*head)->prev;

	return object;
}

list_node_o *list_node_prev(list_node_o **head)
{
	if (*head == NULL)
		return NULL;

	*head = (*head)->prev;

	return *head;
}