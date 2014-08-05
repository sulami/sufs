/*
 * Header for SuFS
 */

/* Magic number */
#define SUFS_MAGIC_NR 0x5c1a31

/* SuFS version, for compability checks */
#define SUFS_VERSION 1;

/* Default block size */
const unsigned int SUFS_DEFAULT_BLOCK_SIZE = (4 * 1024);

struct sufs_super_block {
	unsigned int version;
	unsigned int magic;
	unsigned int block_size;
	unsigned int free_blocks;
	char padding[(4 * 1024) - (4 * sizeof(unsigned int))];
};

