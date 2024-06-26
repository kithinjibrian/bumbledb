#include "fs/vfs/vfs.h"

static fs_o *fs_g = NULL;

fs_o *get_fs()
{
	if (!fs_g)
		error("fun get_fs(): There is no file system!", LOG | PRINT | PANIC);

	return fs_g;
}

void set_fs(fs_o *fs)
{
	fs_g = fs;
	pool_globals_add(fs);
}

void register_fs(fs_o *fs)
{
	SMART dentry_o *entry_root = fs->mount();
	SMART string_o *root = string_from(entry_root->name);
	SMART string_o *parent = clone(root);

	set_fs(fs);

	dcache_save(string_push(MOVE(parent), MOVE(root)), MOVE(entry_root));
}

void unregister_fs()
{
	fs_o *fs = get_fs();
	fs->unmount();
}