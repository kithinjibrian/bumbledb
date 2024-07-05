#pragma once
#ifndef BALLOC_H
#define BALLOC_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdbool.h>

typedef void *object_o;
typedef struct object_header_t object_header_t;

typedef void (*fun_process_t)(object_header_t *, object_o);

typedef void (*fun_free_t)(object_header_t *);

#define TYPES(TYPE)            \
	TYPE(NT_NONE_O)            \
	TYPE(NT_INT_O)             \
	TYPE(NT_NULL_O)            \
	TYPE(NT_FILE_O)            \
	TYPE(NT_ARRAY_O)           \
	TYPE(NT_ERROR_O)           \
	TYPE(NT_BYTES_O)           \
	TYPE(NT_CLASS_O)           \
	TYPE(NT_STRING_O)          \
	TYPE(NT_RSTRING_O)         \
	TYPE(NT_POINTER_O)         \
	TYPE(NT_FUNCTION_O)        \
	TYPE(NT_KEYVALUE_O)        \
	TYPE(NT_LIST_NODE_O)       \
	TYPE(NT_HASH_TABLE_O)      \
	TYPE(NT_ARRAY_OBJECTS_O)   \
	TYPE(FS_O)                 \
	TYPE(TASK_O)               \
	TYPE(DISK_O)               \
	TYPE(NTREE_O)              \
	TYPE(TOKEN_O)              \
	TYPE(VFILE_O)              \
	TYPE(INODE_O)              \
	TYPE(LEXER_O)              \
	TYPE(FRAME_O)              \
	TYPE(DENTRY_O)             \
	TYPE(CLIENT_O)             \
	TYPE(SERVER_O)             \
	TYPE(AST_NODE_O)           \
	TYPE(YARN_POOL_O)          \
	TYPE(LRU_CACHE_O)          \
	TYPE(SUPERBLOCK_O)         \
	TYPE(REPLY_PROTO_O)        \
	TYPE(CACHED_DATA_O)        \
	TYPE(REPLY_HEADER_O)       \
	TYPE(CLIENT_CRYPTO_KEYS_O) \
	TYPE(SERVER_CRYPTO_KEYS_O)

#define GENERATE_ENUM_DT(ENUM) ENUM,
#define GENERATE_STRING_DT(STRING) #STRING,

typedef enum
{
	TYPES(GENERATE_ENUM_DT)
} data_type_e;

typedef void (*fun_deinit_t)(object_o);
typedef object_o (*fun_str_t)(object_o);
typedef object_o (*fun_clone_t)(object_o);
typedef bool (*fun_equals_t)(object_o, object_o);
typedef object_o (*fun_add_t)(object_o, object_o);
typedef object_o (*fun_mult_t)(object_o, object_o);
typedef object_o (*fun_minus_t)(object_o, object_o);
typedef object_o (*fun_divide_t)(object_o, object_o);
typedef object_o (*fun_modulo_t)(object_o, object_o);
typedef object_o (*fun_lesser_t)(object_o, object_o);
typedef object_o (*fun_greater_t)(object_o, object_o);
typedef void (*fun_visitor_t)(object_o, fun_process_t);

typedef struct vtable_t
{
	fun_add_t __add__;
	fun_str_t __str__;
	fun_mult_t __mult__;
	fun_minus_t __minus__;
	fun_clone_t __clone__;
	fun_modulo_t __modulo__;
	fun_divide_t __divide__;
	fun_deinit_t __deinit__;
	fun_equals_t __equals__;
	fun_lesser_t __lesser__;
	fun_greater_t __greater__;
	fun_visitor_t __visitor__;
} vtable_t;

typedef struct data_info_t
{
	int id;
	int ref;
	int count;
	size_t size;
	data_type_e data_type;
} data_info_t;

struct object_header_t
{
	object_o object; /*should be first*/
	int magic_number;
	pthread_mutex_t lock;
	data_info_t data_info;
	const vtable_t *vtable;
	struct object_header_t *next;
};

/*
	Allocation functions
*/
void *b_ralloc(object_o object, int count, size_t size);
void *b_alloc(int count, size_t size, data_type_e data_type);

/*
reference functions
*/
void b_retain(object_o object);
void b_release(object_o object, fun_free_t free);

/*
get the object header
*/
object_o get_object(object_header_t *object_header);
object_header_t *get_object_header(object_o object);

#endif
