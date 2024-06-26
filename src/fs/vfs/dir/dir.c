#include "fs/vfs/vfs.h"

long *dir_make(const char *path)
{
	fs_o *fs = get_fs();

	SMART string_o *key = NULL;
	SMART vfile_o *file = NULL;
	SMART dentry_o *dentry = NULL;
	SMART array_o *path_arr = NULL;
	SMART inode_o *inode_child = NULL;

	path_arr = path_split(path);

	file = lookup(share(path_arr));

	if (file)
	{
		return error("fun dir_make(): Directory already exist!", SILENT);
	}

	SMART string_o *child = (string_o *)array_pop(path_arr);

	file = lookup(share(path_arr));

	if (is_error(file))
		return (void *)file;

	int inode_num = fs->inode_alloc();
	int block_num = fs->data_block_alloc();

	dentry = fs->dir_make_entry(cstring(child), inode_num, FT_DIR);

	dentry_insert(file->inode, dentry);

	inode_child = fs->new_inode(3 * sizeof(dentry_o));
	inode_child->direct_pointers[0] = block_num;
	vnode_write(MOVE(inode_child), inode_num);

	fs->dir_make_empty(block_num, inode_num, file->dentry->inode_number);

	key = dcache_key(string_from(file->dentry->name), MOVE(child));

	dcache_save(MOVE(key), MOVE(dentry));

	return number(true);
}