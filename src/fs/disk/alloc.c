#include "fs/disk/disk.h"

int data_block_alloc(void)
{
	disk_o *disk = disk_get();

	uint8_t bitmap[disk->superblock->block_size];
	memset(bitmap, 0, disk->superblock->block_size);

	block_read(bitmap, disk->superblock->first_data_bitmap_block, disk);

	int free_block_num = bit_search(bitmap, disk->superblock->block_size, 0);

	bit_set(bitmap, free_block_num);

	block_write(bitmap, disk->superblock->first_data_bitmap_block, disk);

	free_block_num += disk->superblock->first_data_block;

	return free_block_num;
}

int inode_alloc(void)
{
	disk_o *disk = disk_get();

	uint8_t bitmap[disk->superblock->block_size];
	block_read(bitmap, disk->superblock->first_inode_bitmap_block, disk);

	int free_inode_num = bit_search(bitmap, disk->superblock->block_size, 0);

	bit_set(bitmap, free_inode_num);

	block_write(bitmap, disk->superblock->first_inode_bitmap_block, disk);

	return free_inode_num;
}