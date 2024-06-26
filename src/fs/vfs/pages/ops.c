#include "fs/vfs/vfs.h"

void page_read(uint8_t *data, int block_num)
{
	fs_o *fs = get_fs();

	uint8_t *buffer = NULL;

	if ((buffer = (uint8_t *)pcache_get(number(block_num))))
	{
		memcpy(data, buffer, fs->block_size);
	}
	else
	{
		fs->block_read(data, block_num, fs->disk_get());
		pcache_save(number(block_num), bytes_from((void *)data, fs->block_size));
	}
}

void page_write(uint8_t *data, int block_num)
{
	fs_o *fs = get_fs();

	uint8_t *buffer = NULL;

	if ((buffer = (uint8_t *)pcache_get(number(block_num))))
	{
		memset(buffer, 0, fs->block_size);
		memcpy(buffer, data, fs->block_size);
	}
	else
	{
		pcache_save(number(block_num), bytes_from((void *)data, fs->block_size));
	}

	fs->block_write(data, block_num, fs->disk_get());
}