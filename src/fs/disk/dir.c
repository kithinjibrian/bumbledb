#include "fs/disk/disk.h"

object_o dentry_str(object_o object)
{
	dentry_o *dentry = (dentry_o *)object;

	return string_from("{ inode: %d, name: \"%s\" }", dentry->inode_number, dentry->name);
}

dentry_o *new_dentry()
{
	dentry_o *dentry;
	dentry = alloc(1, sizeof(dentry_o), DENTRY_O);

	static const vtable_t vt = {.__str__ = dentry_str};

	object_reg_dunders(dentry, &vt);

	return dentry;
}

dentry_o *dir_make_entry(char *name, int inode_num, file_type_e file_type)
{
	dentry_o *dentry = new_dentry();

	dentry->inode_number = inode_num;
	dentry->file_type = file_type;
	dentry->rec_len = sizeof(dentry_o);
	memset(dentry->name, 0, 20);
	strncpy(dentry->name, name, strlen(name));

	return dentry;
}

dentry_o *dir_dot(int inode_num)
{
	dentry_o *dentry = new_dentry();

	dentry->inode_number = inode_num;
	dentry->file_type = FT_DIR;
	dentry->rec_len = sizeof(dentry_o);
	memset(dentry->name, 0, 20);

	strncpy(dentry->name, ".", 2);

	return dentry;
}

dentry_o *dir_dot_dot(int inode_num)
{
	dentry_o *dentry = new_dentry();

	dentry->inode_number = inode_num;
	dentry->file_type = FT_DIR;
	dentry->rec_len = sizeof(dentry_o);
	memset(dentry->name, 0, 20);

	strncpy(dentry->name, "..", 3);

	return dentry;
}

dentry_o *dir_red_light()
{
	dentry_o *dentry = new_dentry();

	dentry->inode_number = 0;
	dentry->file_type = FT_NULL;
	dentry->rec_len = sizeof(dentry_o);
	memset(dentry->name, 0, 20);
	strncpy(dentry->name, "red.light", 11);

	return dentry;
}

void dir_make_empty(int block_num, int inode_num, int inode_parent_num)
{
	disk_o *disk = disk_get();

	SMART dentry_o *dentry_dot = dir_dot(inode_num);
	SMART dentry_o *dentry_dot_dot = dir_dot_dot(inode_parent_num);
	SMART dentry_o *dentry_red_light = dir_red_light();

	uint8_t buffer[disk->superblock->block_size];
	memset(buffer, 0, disk->superblock->block_size);

	memcpy(buffer, dentry_dot, sizeof(dentry_o));
	memcpy(buffer + sizeof(dentry_o), dentry_dot_dot, sizeof(dentry_o));
	memcpy(buffer + (2 * sizeof(dentry_o)), dentry_red_light, sizeof(dentry_o));

	block_write(buffer, block_num, disk);
}