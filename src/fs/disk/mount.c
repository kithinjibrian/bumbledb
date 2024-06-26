#include "fs/disk/disk.h"

static disk_o *disk_g = NULL;

static void __mount_null__(void);
static void __mount_root__(void);
static void __mount_delete__(void);

disk_o *__disk__(file_o *file, superblock_o *superblock)
{
	SMART file_o *smt_file = file;
	SMART superblock_o *smt_superblock = superblock;

	if (disk_g == NULL)
	{
		disk_g = alloc(1, sizeof(disk_o), DISK_O);
		disk_g->file = MOVE(smt_file);
		disk_g->superblock = MOVE(smt_superblock);

		return disk_g;
	}
	return disk_g;
}

disk_o *disk_get(void)
{
	disk_o *disk = __disk__(NULL, NULL);

	if (disk == NULL)
		return error("fun disk_get(): failed to init disk!", LOG | PRINT | PANIC);

	return disk;
}

dentry_o *mount()
{
	file_o *file = f_open(DISK, "rb+");

	if (is_error(file))
	{
		DROP(file);
		file = f_open(DISK, "wb+");
		if (is_error(file))
		{
			error("fun mount(): Error creating file!", LOG | PRINT | PANIC);
		}
	}

	disk_o *disk;
	dentry_o *entry_root = dir_make_entry("/", 2, FT_DIR);
	superblock_o *superblock = superblock_get(file);

	if (superblock->magic == 0xAAAABBBB)
	{
		__disk__(file, superblock);
	}
	else
	{
		DROP(superblock);

		superblock = new_superblock();
		disk = __disk__(MOVE(file), share(superblock));

		uint8_t buffer[superblock->block_size];
		memset(buffer, 0, superblock->block_size);
		memcpy(buffer, superblock, sizeof(superblock_o));
		block_write(buffer, 0, disk);

		memset(buffer, 0, superblock->block_size);
		for (int i = 0; i < superblock->blocks_count - 1; i++)
		{
			block_write(buffer, i + 1, disk);
		}

		DROP(superblock);

		__mount_null__();
		__mount_delete__();
		__mount_root__();
	}

	return entry_root;
}

static void __mount_null__(void)
{
	inode_alloc();
}

static void __mount_delete__(void)
{
	inode_alloc();
}

static void __mount_root__(void)
{
	int inode_root_num = inode_alloc();
	SMART inode_o *inode = new_inode(3 * sizeof(dentry_o));

	/* axiom: 3 dentry structures can't occupy more that one block */
	inode->direct_pointers[0] = data_block_alloc();

	dir_make_empty(inode->direct_pointers[0], inode_root_num, inode_root_num);

	inode_write(inode, inode_root_num);
}

void unmount(void)
{
	disk_o *disk = disk_get();
	DROP(disk->file);
	DROP(disk->superblock);
	DROP(disk);
}