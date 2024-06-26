#pragma once
#ifndef VFS_H
#define VFS_H

#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "cache/cache.h"
#include "utils/bool/bool.h"
#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "objects/natives/natives.h"

#define READ (1 << 0)
#define WRITE (1 << 1)
#define APPEND (1 << 2)
#define READ_WRITE (1 << 3)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef lru_cache_o dcache_o;
typedef lru_cache_o icache_o;
typedef lru_cache_o pcache_o;

typedef struct disk_o disk_o;
typedef struct inode_o inode_o;
typedef struct dentry_o dentry_o;
typedef struct superblock_o superblock_o;

typedef void (*fun_unmount_t)(void);
typedef dentry_o *(*fun_mount_t)(void);
typedef int (*fun_inode_alloc_t)(void);
typedef disk_o *(*fun_disk_get_t)(void);
typedef dentry_o *(*fun_dir_dot_t)(int);
typedef dentry_o *(*fun_new_dentry_t)();
typedef inode_o *(*fun_new_inode_t)(int);
typedef inode_o *(*fun_inode_get_t)(int);
typedef dentry_o *(*fun_dir_dot_dot_t)(int);
typedef int (*fun_data_block_alloc_t)(void);
typedef dentry_o *(*fun_dir_red_light_t)(void);
typedef void (*fun_inode_read_t)(inode_o *, int);
typedef void (*fun_inode_write_t)(inode_o *, int);
typedef void (*fun_dentry_t)(dentry_o *, object_o);
typedef superblock_o *(*fun_new_superblock_t)(void);
typedef void (*fun_dir_make_empty_t)(int, int, int);
typedef bool (*fun_dentry_find_t)(dentry_o *, object_o);
typedef superblock_o *(*fun_superblock_get_t)(file_o *);
typedef void (*fun_block_read_t)(uint8_t *, int, disk_o *);
typedef void (*fun_block_write_t)(uint8_t *, int, disk_o *);

typedef enum
{
	FT_NULL = 0,
	FT_REG_FILE,
	FT_DIR
} file_type_e;

typedef dentry_o *(*fun_dir_make_entry_t)(char *, int, file_type_e);

#pragma pack(push, 1)

struct superblock_o
{
	uint32_t inodes_count;			   /*number of inodes*/
	uint32_t inode_size;			   /*size of inodes*/
	uint32_t blocks_count;			   /*number of blocks*/
	uint32_t block_size;			   /*size of blocks*/
	uint32_t first_inode_bitmap_block; /*first inode bitmap block*/
	uint32_t first_data_bitmap_block;  /*first data bitmap block*/
	uint32_t first_inode_block;		   /*first inode block*/
	uint32_t first_data_block;		   /*first data block*/
	uint32_t magic;
};

struct dentry_o
{
	uint32_t inode_number;
	uint32_t rec_len;
	file_type_e file_type;
	char name[20];
};

struct inode_o
{
	uint32_t size;
	uint32_t direct_pointers[4];
};

#pragma pack(pop)

struct disk_o
{
	file_o *file;
	superblock_o *superblock;
};

typedef struct fs_o
{
	int nodp; /*number of direct pointers*/
	int block_size;
	int max_file_size;
	const char *fs_name;
	/*
	methods
	*/
	fun_mount_t mount;
	fun_dir_dot_t dir_dot;
	fun_unmount_t unmount;
	fun_disk_get_t disk_get;
	fun_inode_get_t inode_get;
	fun_new_inode_t new_inode;
	fun_inode_read_t inode_read;
	fun_block_read_t block_read;
	fun_new_dentry_t new_dentry;
	fun_dir_dot_dot_t dir_dot_dot;
	fun_inode_alloc_t inode_alloc;
	fun_block_write_t block_write;
	fun_inode_write_t inode_write;
	fun_dir_red_light_t dir_red_light;
	fun_dir_make_entry_t dir_make_entry;
	fun_new_superblock_t new_superblock;
	fun_superblock_get_t superblock_get;
	fun_dir_make_empty_t dir_make_empty;
	fun_data_block_alloc_t data_block_alloc;
} fs_o;

typedef struct vfile_o
{
	int mode;
	int offset;
	inode_o *inode;
	dentry_o *dentry;
} vfile_o;

typedef struct cached_data_o
{
	/*private data*/
	bool __is_dirty__;

	/*public data*/
	object_o object;

} cached_data_o;

/*
 un/register fs
*/
fs_o *get_fs();
void unregister_fs();
void register_fs(fs_o *fs);

/*
	dcache
*/
void dcache_free();
void dcache_print();
dcache_o *dcache_init(void);
object_o dcache_get(object_o key);
void dcache_save(object_o key, object_o value);
string_o *dcache_key(string_o *parent, string_o *child);

/*
 */
void dentry_insert(inode_o *inode, dentry_o *dentry);
void dentry_foreach(fun_dentry_t fun_dentry, object_o args, inode_o *inode);
dentry_o *dentry_find(fun_dentry_t fun_dentry, object_o args, string_o *target, inode_o *inode);

/*
	inode
*/
void icache_free();
void icache_print();
icache_o *icache_init(void);
inode_o *vnode_read(int inode_num);
object_o icache_get(object_o key);
void icache_save(object_o key, object_o value);
void vnode_write(inode_o *inode, int inode_num);

/*
	page cache
*/
void pcache_free();
void pcache_print();
icache_o *pcache_init(void);
object_o pcache_get(object_o key);
void page_read(uint8_t *data, int block_num);
void page_write(uint8_t *data, int block_num);
void pcache_save(object_o key, uint8_t *value);

/*
cdo
*/
void cdo_set_dirty(cached_data_o *cdo);
bool cdo_get_dirty(cached_data_o *cdo);
cached_data_o *create_cached_object(object_o object, bool dirty);

/*
	path
*/
array_o *path_split(const char *path);

/*
	path lookup
*/
vfile_o *lookup(array_o *path);

long *dir_make(const char *path);

int vsize(vfile_o *self);
int vtell(vfile_o *self);
void vrewind(vfile_o *self);
void vseek(vfile_o *self, int offset);
vfile_o *vopen(const char *filename, int mode);
vfile_o *new_vfile(inode_o *inode, dentry_o *dentry);
long *vread(vfile_o *self, void *data, int count, size_t size);
long *vwrite(vfile_o *self, void *data, int count, size_t size);

#endif