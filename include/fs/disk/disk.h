#pragma once
#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stdbool.h>

#include "fs/vfs/vfs.h"
#include "objects/pool/pool.h"
#include "objects/pool/alloc.h"
#include "utils/bitmap/bitmap.h"
#include "objects/natives/natives.h"

#define DISK "disk.dat"
#define BLOCK_SIZE 1024

/*
 */
fs_o *simple_fs();

dentry_o *mount();
void unmount(void);
disk_o *disk_get(void);

/*
	superblock
*/
superblock_o *new_superblock(void);
superblock_o *superblock_get(file_o *disk);

/*
	read and write
*/
void inode_read(inode_o *inode, int inode_num);
void inode_write(inode_o *inode, int inode_num);
//
void block_read(uint8_t *data, int block_num, disk_o *disk);
void block_write(uint8_t *data, int block_num, disk_o *disk);

/*
	allocation
*/
int inode_alloc(void);
int data_block_alloc(void);

/*
	Inode
*/
inode_o *new_inode(int size);
inode_o *inode_get(int inode_num);
void inode_print(object_o object, int depth);

/*
	dirs
*/
dentry_o *new_dentry();
dentry_o *dir_red_light();
dentry_o *dir_dot(int inode_num);
dentry_o *dir_dot_dot(int inode_num);
void dir_make_empty(int block_num, int inode_num, int inode_parent_num);
dentry_o *dir_make_entry(char *name, int inode_num, file_type_e file_type);

#endif