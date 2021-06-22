/*
 * ext2root.c
 *
 * Read root directory inode (#2) from the inode table of the disk.
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
#define BLOCK_OFFSET(block) ((block)*block_size)

static unsigned int block_size = 0; /* block size (to be calculated) */

static void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode)
{
	lseek(fd, BLOCK_OFFSET(group->bg_inode_table) + inode_no * sizeof(struct ext2_inode), SEEK_SET);
	read(fd, inode, sizeof(struct ext2_inode));
} /* read_inode() */

int isZero(int len, void *ptr)
{
	char *bptr = (char *)ptr;
	while (len--)
		if (*bptr++)
			return 0;
	return 1;
}

int main(int argc, char *argv[])
{
	int groupNum = 0, inodeNum = 2;
	if (argc >= 2)
		groupNum = atoi(argv[1]);
	if (argc == 3)
		inodeNum = atoi(argv[2]);

	struct ext2_super_block super;
	struct ext2_group_desc group;
	struct ext2_inode inode;
	int fd;
	int i;

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

	lseek(fd, block_size + groupNum * sizeof(group), SEEK_SET);
	read(fd, &group, sizeof(group));

	/* read root inode */
	read_inode(fd, inodeNum, &group, &inode);

	printf("Reading inode\n"
		   "File mode: %hu\n"
		   "Owner UID: %hu\n"
		   "Size     : %u bytes\n"
		   "Blocks   : %u\n"
		   "atime    : %d\n"
		   "ctime    : %d\n"
		   "mtime    : %d\n"
		   "dtime    : %d\n"
		   "gid      : %d\n"
		   "links_cnt: %d\n"
		   "flags    : %d\n",
		   inode.i_mode,
		   inode.i_uid,
		   inode.i_size,
		   inode.i_blocks,
		   inode.i_atime,
		   inode.i_ctime,
		   inode.i_mtime,
		   inode.i_dtime,
		   inode.i_gid,
		   inode.i_links_count,
		   inode.i_flags);

	for (i = 0; i < EXT2_N_BLOCKS; i++)
		if (i < EXT2_NDIR_BLOCKS) /* direct blocks */
			printf("Block %2u : %u\n", i, inode.i_block[i]);
		else if (i == EXT2_IND_BLOCK) /* single indirect block */
			printf("Single   : %u\n", inode.i_block[i]);
		else if (i == EXT2_DIND_BLOCK) /* double indirect block */
			printf("Double   : %u\n", inode.i_block[i]);
		else if (i == EXT2_TIND_BLOCK) /* triple indirect block */
			printf("Triple   : %u\n", inode.i_block[i]);
	printf("\n");

	close(fd);
	exit(0);
} /* main() */