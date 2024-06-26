#include "fs/vfs/vfs.h"

string_o *dcache_key(string_o *parent, string_o *child)
{
	return string_push(parent, child);
}

dentry_o *dentry_find(fun_dentry_t fun_dentry, object_o args, string_o *target, inode_o *inode)
{
	fs_o *fs = get_fs();

	SMART object_o smt_args = args;
	SMART string_o *smt_target = target;
	SMART inode_o *smt_inode = inode;

	for (int i = 0; i < fs->nodp; i++)
	{
		int block_num = inode->direct_pointers[i];

		if (block_num == 0)
		{
			continue;
		}

		uint8_t buffer[fs->block_size];
		page_read(buffer, block_num);

		uint8_t *ptr = buffer;
		while (ptr < buffer + sizeof(buffer))
		{
			dentry_o *dentry = (dentry_o *)ptr;

			if (dentry->file_type == FT_NULL)
			{
				return NULL;
			}

			SMART dentry_o *new_dentry = fs->new_dentry();
			memcpy(new_dentry, dentry, sizeof(dentry_o));

			fun_dentry(share(new_dentry), share(smt_args));

			if (strcmp(cstring(smt_target), dentry->name) == 0)
			{
				RETURN(new_dentry);
			}

			ptr += sizeof(dentry_o);
		}
	}

	return NULL;
}

void dentry_foreach(fun_dentry_t fun_dentry, object_o args, inode_o *inode)
{
	fs_o *fs = get_fs();

	SMART dentry_o *dentry = NULL;
	SMART object_o smt_args = args;
	SMART inode_o *smt_inode = inode;

	for (int i = 0; i < 4; i++)
	{
		int block_num = inode->direct_pointers[i];

		uint8_t buffer[fs->block_size];
		page_read(buffer, block_num);

		uint8_t *ptr = buffer;
		while (((dentry_o *)ptr)->file_type != FT_NULL)
		{
			dentry = alloc(1, sizeof(dentry_o), DENTRY_O);
			memcpy(dentry, ((dentry_o *)ptr), sizeof(dentry_o));

			fun_dentry(MOVE(dentry), share(smt_args));

			ptr += sizeof(dentry_o);
		}
	}
}

void dentry_insert(inode_o *inode, dentry_o *dentry)
{
	fs_o *fs = get_fs();

	uint8_t buffer[fs->block_size];
	int block_num = 0, red_light = 0;
	int max_dentries = fs->block_size / sizeof(dentry_o);
	SMART dentry_o *dentry_red_light = fs->dir_red_light();
	SMART dentry_o *dentry_to_write = alloc(2, sizeof(dentry_o), DENTRY_O);

	memcpy(dentry_to_write, dentry, sizeof(dentry_o));
	memcpy(dentry_to_write + 1, dentry_red_light, sizeof(dentry_o));

	for (block_num = 0; block_num < fs->nodp; block_num++)
	{
		memset(buffer, 0, fs->block_size);
		page_read(buffer, inode->direct_pointers[block_num]);

		uint8_t *ptr = buffer;

		red_light = 0;

		while (ptr < buffer + fs->block_size)
		{
			if (((dentry_o *)ptr)->file_type == FT_NULL)
			{
				goto jump_here;
			}

			red_light++;
			ptr += sizeof(dentry_o);
		}
	}

jump_here:
	if (red_light == (max_dentries - 1))
	{
		uint8_t buffer_next[fs->block_size];

		if (inode->direct_pointers[block_num + 1] == 0)
		{
			inode->direct_pointers[block_num + 1] = fs->data_block_alloc();
		}

		page_read(buffer_next, inode->direct_pointers[block_num + 1]);

		memcpy(buffer + (red_light * sizeof(dentry_o)), dentry_to_write, sizeof(dentry_o));
		memcpy(buffer_next, dentry_to_write + 1, sizeof(dentry_o));

		page_write(buffer_next, inode->direct_pointers[block_num + 1]);
	}
	else
	{
		memcpy(buffer + (red_light * sizeof(dentry_o)), dentry_to_write, 2 * sizeof(dentry_o));
	}

	page_write(buffer, inode->direct_pointers[block_num]);
}