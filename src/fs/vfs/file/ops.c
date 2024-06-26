#include "fs/vfs/vfs.h"

void vfile_visitor(object_o object, fun_process_t fun_process)
{
	vfile_o *file = (vfile_o *)object;

	if (file->inode)
		post_visit(file->inode, fun_process);

	if (file->dentry)
		post_visit(file->dentry, fun_process);
}

void vfile_print(object_o object, int depth)
{
	vfile_o *file = (vfile_o *)object;

	char space[depth];
	memset(space, ' ', depth);
	space[depth] = '\0';

	printf(YELLOW "<vfile>");

	printf(RESET ": {\n %sinode: ", space);
	// print_offset(file->inode, depth);

	printf(RESET " %sdentry: ", space);
	// print_offset(file->dentry, depth);

	printf(RESET " %smode: ", space);
	// printf(GREEN "%d\n" RESET, file->mode);

	printf(RESET " %soffset: ", space);
	printf(GREEN "%d\n" RESET, file->offset);

	printf("%s}", space);
}

vfile_o *new_vfile(inode_o *inode, dentry_o *dentry)
{
	SMART vfile_o *file = alloc(1, sizeof(vfile_o), VFILE_O);
	file->offset = 0;
	file->mode = READ;
	file->inode = inode;
	file->dentry = dentry;

	static const vtable_t vt = {
		.__print__ = vfile_print,
		.__visitor__ = vfile_visitor};

	object_reg_dunders(file, &vt);

	RETURN(file);
}

vfile_o *vopen(const char *filename, int mode)
{
	fs_o *fs = get_fs();

	SMART string_o *key = NULL;
	SMART vfile_o *file = NULL;
	SMART dentry_o *dentry = NULL;
	SMART inode_o *inode_child = NULL;

	SMART array_o *path = path_split(filename);

	file = lookup(share(path));

	if (file)
	{
		file->mode = mode;
		RETURN(file);
	}

	if (file == NULL && mode & READ)
	{
		return error("fun vopen(): File does not exist!", SILENT);
	}

	SMART string_o *child = (string_o *)array_pop(path);
	SMART vfile_o *file_parent = lookup(share(path));

	int inode_num = fs->inode_alloc();
	inode_child = fs->new_inode(0);
	vnode_write(share(inode_child), inode_num);

	dentry = fs->dir_make_entry(cstring(child), inode_num, FT_REG_FILE);
	dentry_insert(file_parent->inode, dentry);

	key = dcache_key(string_from(file_parent->dentry->name), MOVE(child));

	dcache_save(MOVE(key), share(dentry));

	file = new_vfile(share(inode_child), share(dentry));
	file->mode = mode;

	RETURN(file);
}

void vseek(vfile_o *self, int offset)
{
	self->offset = offset;
}

void vrewind(vfile_o *self)
{
	self->offset = 0;
}

int vtell(vfile_o *self)
{
	return self->offset;
}

int vsize(vfile_o *self)
{
	return self->inode->size;
}

void truncate(vfile_o *self)
{
	int size = self->inode->size;
	int mode = self->mode;

	uint8_t buffer[size];
	memset(buffer, 0, size);

	self->offset = 0;
	self->mode = READ_WRITE;
	SMART long *bw = vwrite(self, buffer, size, sizeof(char));

	self->offset = 0;
	self->mode = mode;
	self->inode->size = 0;

	vnode_write(share(self->inode), self->dentry->inode_number);
}

long *vwrite(vfile_o *self, void *data, int count, size_t size)
{
	fs_o *fs = get_fs();

	int bytes_written = 0;
	int total_size = count * size;

	if (self->mode & READ)
		return error("fun vwrite(): File opened in read-only mode.", SILENT);

	if (self->mode & WRITE)
	{
		truncate(self);
	}

	if (self->mode & APPEND)
		vseek(self, self->inode->size);

	if (self->offset < 0 || (self->offset + total_size) > (fs->max_file_size))
		return error("fun vwrite(): Offset is out of file bounds.", SILENT);

	while (total_size > 0)
	{
		int block_index = floor(self->offset / fs->block_size);
		int block_offset = self->offset % fs->block_size;
		int space_in_block = fs->block_size - block_offset;

		int bytes_to_write = MIN(total_size, space_in_block);

		bytes_written += bytes_to_write;

		if (self->inode->direct_pointers[block_index] == 0)
		{
			int data_block_num = fs->data_block_alloc();
			self->inode->direct_pointers[block_index] = data_block_num;
		}

		uint8_t block_data[fs->block_size];
		page_read(block_data, self->inode->direct_pointers[block_index]);

		memcpy(block_data + block_offset, data, bytes_to_write);

		page_write(block_data, self->inode->direct_pointers[block_index]);

		self->offset += bytes_to_write;
		data = (void *)((char *)data + bytes_to_write);
		total_size -= bytes_to_write;
	}

	if (self->offset > self->inode->size)
	{
		self->inode->size = self->offset;
	}

	vnode_write(share(self->inode), self->dentry->inode_number);

	return number(bytes_written);
}

long *vread(vfile_o *self, void *data, int count, size_t size)
{
	if (!(self->mode & READ || self->mode & READ_WRITE || self->mode & APPEND))
	{
		return error("fun vread(): File not opened in read mode.", SILENT);
	}

	fs_o *fs = get_fs();

	int bytes_read = 0;
	int total_size = count * size;

	if (self->offset < 0 || (self->offset + total_size) > (self->inode->size))
		return error("fun vread(): Offset is out of file bounds.", SILENT);

	while (total_size > 0)
	{
		int block_index = floor(self->offset / fs->block_size);
		int block_offset = self->offset % fs->block_size;
		int space_in_block = fs->block_size - block_offset;

		int block_num = self->inode->direct_pointers[block_index];

		int bytes_to_read = MIN(total_size, space_in_block);
		bytes_read += bytes_to_read;

		uint8_t block_data[fs->block_size];
		page_read(block_data, block_num);

		memcpy(data, block_data + block_offset, bytes_to_read);

		self->offset += bytes_to_read;
		data = (void *)((char *)data + bytes_to_read);
		total_size -= bytes_to_read;
	}

	return number(bytes_read);
}