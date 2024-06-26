#include "fs/disk/disk.h"

void block_write(uint8_t *data, int block_num, disk_o *disk)
{
	int true_offset = disk->superblock->block_size * block_num;
	fseek(f_get(disk->file), true_offset, SEEK_SET);
	fwrite(data, sizeof(uint8_t), disk->superblock->block_size, f_get(disk->file));
	fflush(f_get(disk->file));
}

void block_read(uint8_t *data, int block_num, disk_o *disk)
{
	int true_offset = disk->superblock->block_size * block_num;
	fseek(f_get(disk->file), true_offset, SEEK_SET);
	fread(data, sizeof(uint8_t), disk->superblock->block_size, f_get(disk->file));
}

void inode_read(inode_o *inode, int inode_num)
{
	disk_o *disk = disk_get();

	uint32_t block_num = (inode_num / disk->superblock->inodes_count) + disk->superblock->first_inode_block;
	uint32_t inode_offset = (inode_num % disk->superblock->inodes_count) * disk->superblock->inode_size;

	uint8_t buffer[disk->superblock->block_size];
	block_read(buffer, block_num, disk);

	memcpy(inode, buffer + inode_offset, sizeof(inode_o));
}

void inode_write(inode_o *inode, int inode_num)
{
	disk_o *disk = disk_get();

	uint32_t block_num = (inode_num / disk->superblock->inodes_count) + disk->superblock->first_inode_block;
	uint32_t inode_offset = (inode_num % disk->superblock->inodes_count) * disk->superblock->inode_size;

	uint8_t buffer[disk->superblock->block_size];
	block_read(buffer, block_num, disk);

	memcpy(buffer + inode_offset, inode, sizeof(inode_o));

	block_write(buffer, block_num, disk);
}
