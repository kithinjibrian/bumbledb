#include "fs/vfs/vfs.h"

void dentry_cache(dentry_o *dentry, object_o args)
{
	SMART string_o *key = NULL;
	SMART string_o *smt_args = (string_o *)args;
	SMART dentry_o *smt_dentry = dentry;

	key = dcache_key(clone(smt_args), string_from(dentry->name));

	dcache_save(MOVE(key), MOVE(smt_dentry));
}

void walk(vfile_o **root_file, array_o *path)
{
	SMART array_o *smt_path = path;

	string_o *target = array_next(smt_path);
	string_o *parent = array_at(smt_path, array_index(smt_path) - 2);

	if (target)
	{
		if ((*root_file)->dentry)
			DROP((*root_file)->dentry);

		(*root_file)->dentry = dentry_find(
			dentry_cache,
			share(parent),
			share(target),
			share((*root_file)->inode));

		if ((*root_file)->dentry)
		{
			DROP((*root_file)->inode);
			(*root_file)->inode = vnode_read((*root_file)->dentry->inode_number);

			walk(root_file, MOVE(smt_path));
		}
		else
		{
			DROP(*root_file);
			return;
		}
	}
}

vfile_o *lookup(array_o *path)
{
	int parent, child;
	dentry_o *dentry = NULL;

	SMART string_o *key = NULL;
	SMART inode_o *inode = NULL;

	SMART array_o *smt_path = path;

	if (array_length(smt_path) == 1)
	{
		dentry_o *d = dcache_get(string_from("//"));
		if (d)
		{
			return new_vfile(vnode_read(2), share(d));
		}
		return error("fun lookup(): Cannot find root dentry!", SILENT | PRINT);
	}
	else
	{
		child = array_length(smt_path) - 1;
		parent = array_length(smt_path) - 2;

		key = dcache_key(
			share(array_at(smt_path, parent)),
			share(array_at(smt_path, child)));

		if ((dentry = (dentry_o *)dcache_get(MOVE(key))))
		{
			inode = vnode_read(dentry->inode_number);

			return new_vfile(MOVE(inode), share(dentry));
		}
		else
		{
			/*
			skip the first path component
			*/
			array_next(smt_path);

			vfile_o *file = new_vfile(vnode_read(2), NULL);

			walk(&file, MOVE(smt_path));

			return file;
		}
	}

	return error("fun lookup(): Can't lookup file!", SILENT);
}