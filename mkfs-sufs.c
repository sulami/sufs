#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "sufs.h"

int main(int argc, char *argv[])
{
	int fd;
	ssize_t retval;
	struct sufs_super_block sb;

	if (argc != 2) {
		printf("Usage: mkfs-sufs <device>\n");
		return -EINVAL;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("Error opening the device\n");
		return fd;
	}

	sb.version = SUFS_VERSION;
	sb.magic = SUFS_MAGIC_NR;
	sb.block_size = SUFS_DEFAULT_BLOCK_SIZE;
	sb.free_blocks = ~0;

	retval = write(fd, (char *)&sb, sizeof(sb));
	if (retval != SUFS_DEFAULT_BLOCK_SIZE)
		printf(
		"Warning: Wrote %d bytes, but default block size is %d\n",
		retval, SUFS_DEFAULT_BLOCK_SIZE);
	else
		printf("Superblock written successfully\n");

	close(fd);

	return 0;
}

