/*
 * ext2group.c
 *
 * Reads the first (and only) group-descriptor from a Ext2 disk.
 *
 * Questions?
 * Emanuele Altieri
 * ealtieri@hampshire.edu
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ext2fs/ext2fs.h>

#define BASE_OFFSET 1024	  /* locates beginning of the super block (first group) */
#define FD_DEVICE "/dev/sda1" /* the device */

static unsigned int block_size = 0; /* block size (to be calculated) */

int isZero(int len, void *ptr)
{
	char *bptr = (char *)ptr;
	while (len--)
		if (*bptr++)
			return 0;
	return 1;
}

int main(void)
{
	struct ext2_super_block super;
	struct ext2_group_desc group;
	int fd;

	/* open device */

	if ((fd = open(FD_DEVICE, O_RDONLY)) < 0)
	{
		perror(FD_DEVICE);
		exit(1); /* error while opening the device */
	}

	/* read super-block */

	lseek(fd, BASE_OFFSET, SEEK_SET);
	read(fd, &super, sizeof(super));

	if (super.s_magic != EXT2_SUPER_MAGIC)
	{
		fprintf(stderr, "Not a Ext2 filesystem\n");
		exit(1);
	}

	block_size = 1024 << super.s_log_block_size;

	/* read group descriptor */

	unsigned int groupNum = 1, group_count = 1 + (super.s_blocks_count - 1) / super.s_blocks_per_group;
	printf("%u %u\n", group_count, super.s_blocks_per_group * block_size);
	while (groupNum <= group_count)
	{
		lseek(fd, block_size + (groupNum++ - 1) * sizeof(group), SEEK_SET);
		read(fd, &group, sizeof(group));
		if (isZero(sizeof(group), &group))
			continue;

		printf("Reading block number %d from device " FD_DEVICE ":\n"
			   "Blocks bitmap block: %u\n"
			   "Inodes bitmap block: %u\n"
			   "Inodes table block : %u\n"
			   "Free blocks count  : %u\n"
			   "Free inodes count  : %u\n"
			   "Directories count  : %u\n\n",
			   groupNum - 1,
			   group.bg_block_bitmap,
			   group.bg_inode_bitmap,
			   group.bg_inode_table,
			   group.bg_free_blocks_count,
			   group.bg_free_inodes_count,
			   group.bg_used_dirs_count); /* directories count */
	}
	close(fd);

	exit(0);
} /* main() */