#include "yarns/yarns.h"

struct yarn_pool_o
{
	bool shutdown;
	size_t yarn_count;
	list_node_o *tasks;
	size_t active_tasks;
	pthread_mutex_t lock;
	pthread_cond_t notify;
	pthread_cond_t not_working;
};

static yarn_pool_o *yarn_pool_g = NULL;

yarn_pool_o *__yarn_pool_init__(void)
{
	if (yarn_pool_g == NULL)
	{
		yarn_pool_g = yarn_pool_init(2);
		pool_globals_add(yarn_pool_g);
	}
	return yarn_pool_g;
}

void *worker(object_o arg)
{
	task_o *task;
	yarn_pool_o *pool;

	pool = (yarn_pool_o *)arg;

	while (1)
	{
		pthread_mutex_lock(&(pool->lock));
		while (pool->tasks == NULL && !pool->shutdown)
		{
			pthread_cond_wait(&(pool->notify), &(pool->lock));
		}

		if (pool->shutdown)
			break;

		task = (task_o *)list_shift(&(pool->tasks));

		pool->active_tasks++;

		pthread_mutex_unlock(&(pool->lock));

		if (task != NULL)
		{
			fun_task_o fun_task = (fun_task_o)task->function->function;
			fun_task(task->args);

			DROP(task);
		}

		pthread_mutex_lock(&(pool->lock));
		pool->active_tasks--;
		if (pool->active_tasks == 0 && pool->tasks == NULL)
			pthread_cond_signal(&(pool->not_working));
		pthread_mutex_unlock(&(pool->lock));
	}

	pool->yarn_count--;
	pthread_cond_signal(&(pool->not_working));
	pthread_mutex_unlock(&(pool->lock));
	return NULL;
}

yarn_pool_o *yarn_pool_init(size_t num_yarns)
{
	size_t i;
	yarn_pool_o *pool;
	pthread_t thread;

	pool = (yarn_pool_o *)alloc(1, sizeof(yarn_pool_o), YARN_POOL_O);

	if (num_yarns == 0)
		num_yarns = 2;

	pool->yarn_count = num_yarns;

	pool->shutdown = 0;
	pool->active_tasks = 0;

	pthread_mutex_init(&(pool->lock), NULL);
	pthread_cond_init(&(pool->notify), NULL);
	pthread_cond_init(&(pool->not_working), NULL);

	pool->tasks = NULL;

	for (i = 0; i < num_yarns; i++)
	{
		pthread_create(&thread, NULL, worker, pool);
		pthread_detach(thread);
	}

	return pool;
}

bool yarn_pool_add_task(function_o *function, object_o args)
{
	yarn_pool_o *pool;
	pool = __yarn_pool_init__();

	task_o *task;

	task = create_task(function, args);
	if (task == NULL)
		return false;

	pthread_mutex_lock(&(pool->lock));

	list_push(&(pool->tasks), task);

	pthread_cond_signal(&(pool->notify));
	pthread_mutex_unlock(&(pool->lock));
	return true;
}

void yarn_pool_wait(void)
{
	yarn_pool_o *pool;
	pool = __yarn_pool_init__();

	pthread_mutex_lock(&(pool->lock));
	while (1)
	{
		if (pool->tasks != NULL || (!pool->shutdown && pool->active_tasks != 0) || (pool->shutdown && pool->yarn_count != 0))
		{
			pthread_cond_wait(&(pool->not_working), &(pool->lock));
		}
		else
		{
			break;
		}
	}
	pthread_mutex_unlock(&(pool->lock));
}

void yarn_pool_destory()
{
	yarn_pool_o *pool;
	pool = __yarn_pool_init__();

	DROP(pool->tasks);

	pthread_mutex_lock(&(pool->lock));

	/*house keeping*/
	pool->shutdown = 1;

	pthread_cond_broadcast(&(pool->notify));
	pthread_mutex_unlock(&(pool->lock));

	yarn_pool_wait();

	pthread_mutex_destroy(&(pool->lock));
	pthread_cond_destroy(&(pool->notify));
	pthread_cond_destroy(&(pool->not_working));
}