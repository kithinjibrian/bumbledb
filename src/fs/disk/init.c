#include "fs/disk/disk.h"

fs_o *simple_fs()
{
	fs_o *fs = alloc(1, sizeof(fs_o), FS_O);
	fs->nodp = 4; // number of direct pointers
	fs->block_size = BLOCK_SIZE;
	fs->fs_name = "simple-fs";
	fs->max_file_size = BLOCK_SIZE * 4;
	/*
	methods
	*/
	fs->mount = mount;
	fs->dir_dot = dir_dot;
	fs->unmount = unmount;
	fs->disk_get = disk_get;
	fs->new_inode = new_inode;
	fs->inode_read = inode_read;
	fs->block_read = block_read;
	fs->new_dentry = new_dentry;
	fs->inode_alloc = inode_alloc;
	fs->inode_write = inode_write;
	fs->block_write = block_write;
	fs->dir_dot_dot = dir_dot_dot;
	fs->dir_red_light = dir_red_light;
	fs->dir_make_entry = dir_make_entry;
	fs->dir_make_empty = dir_make_empty;
	fs->data_block_alloc = data_block_alloc;

	return fs;
}