#include "fs/disk/disk.h"

inode_o *inode_get(int inode_num)
{
	inode_o *inode;

	inode = alloc(1, sizeof(inode_o), INODE_O);

	inode_read(inode, inode_num);

	return inode;
}

inode_o *new_inode(int size)
{
	inode_o *inode;
	inode = alloc(1, sizeof(inode_o), INODE_O);
	inode->size = size;

	for (int a = 0; a < 4; a++)
	{
		inode->direct_pointers[a] = 0;
	}

	static const vtable_t vt = {.__print__ = inode_print};

	object_reg_dunders(inode, &vt);

	return inode;
}

void inode_print(object_o object, int depth)
{
	inode_o *inode = (inode_o *)object;

	char space[depth];
	memset(space, ' ', depth);
	space[depth] = '\0';

	printf(YELLOW "<inode>");
	printf(RESET ": {\n %ssize :" GREEN " %d\n" RESET, space, inode->size);

	for (int a = 0; a < 4; a++)
	{
		if (inode->direct_pointers[a] == 0)
		{
			printf(" %sblock %d: " GREEN "%s\n" RESET, space, a, "None");
		}
		else
		{
			printf(" %sblock %d: " GREEN "%d\n" RESET, space, a, inode->direct_pointers[a]);
		}
	}
	printf("%s}", space);
}