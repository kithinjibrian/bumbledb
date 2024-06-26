#include "fs/vfs/vfs.h"

inode_o *vnode_read(int inode_num)
{
	fs_o *fs = get_fs();
	inode_o *inode = NULL;

	if ((inode = (inode_o *)icache_get(number(inode_num))))
	{
		share(inode);
	}
	else
	{
		inode = fs->new_inode(0);
		fs->inode_read(inode, inode_num);
		icache_save(number(inode_num), share(inode));
	}

	return inode;
}

void vnode_write(inode_o *inode, int inode_num)
{
	fs_o *fs = get_fs();

	SMART inode_o *smt_inode = inode;

	if (!(icache_get(number(inode_num))))
	{
		icache_save(number(inode_num), share(smt_inode));
	}

	fs->inode_write(smt_inode, inode_num);
}