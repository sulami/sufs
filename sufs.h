#include "sufs_btree.h"

/* Magic number */
#define SUFS_MAGIC_NR 0x5c1a31

/* SuFS version, for compability checks */
#define SUFS_VERSION 2;

/* Default block size */
const uint32_t SUFS_DEFAULT_BLOCK_SIZE = (4 * 1024);

/* Super block layout */
struct sufs_super_block {
	uint32_t version;
	uint32_t magic;
	uint32_t block_size;
	uint32_t free_blocks;
	struct node *root_node;
	char padding[(4 * 1024) - (4 * sizeof(uint32_t))];
};

