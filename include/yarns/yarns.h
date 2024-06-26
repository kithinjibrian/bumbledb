#pragma once
#ifndef YARN_H
#define YARN_H

#include "objects/object.h"

typedef void (*fun_task_o)(object_o);
typedef struct yarn_pool_o yarn_pool_o;

typedef struct task_o
{
	object_o args;
	function_o *function;
} task_o;

void yarn_pool_wait(void);
void yarn_pool_destory(void);
yarn_pool_o *yarn_pool_init(size_t num_yarns);
task_o *create_task(function_o *function, object_o args);
bool yarn_pool_add_task(function_o *function, object_o args);

#endif