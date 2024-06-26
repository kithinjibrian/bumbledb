#include "fs/disk/disk.h"

superblock_o *superblock_get(file_o *disk)
{
	superblock_o *superblock = NULL;
	superblock = alloc(1, sizeof(superblock_o), SUPERBLOCK_O);

	fseek(f_get(disk), 0, SEEK_SET);
	fread(superblock, sizeof(superblock_o), 1, f_get(disk));

	return superblock;
}

superblock_o *new_superblock(void)
{
	superblock_o *superblock = NULL;
	superblock = alloc(1, sizeof(superblock_o), SUPERBLOCK_O);

	superblock->block_size = BLOCK_SIZE;
	superblock->blocks_count = 20;
	superblock->inode_size = sizeof(inode_o);
	superblock->inodes_count = (BLOCK_SIZE / sizeof(inode_o));
	superblock->magic = 0xAAAABBBB;
	superblock->first_inode_bitmap_block = 1;
	superblock->first_data_bitmap_block = 2;
	superblock->first_inode_block = 3;
	superblock->first_data_block = 4;

	return superblock;
}