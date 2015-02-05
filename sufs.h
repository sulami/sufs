#include "sufs_btree.h"

/* Magic number */
#define SUFS_MAGIC_NR 0x5c1a31

/* SuFS version, for compability checks */
#define SUFS_VERSION 3

/* Default blocksize */
#define SUFS_DEFAULT_BLOCK_SIZE 4096

/* Max length of filenames */
#define SUFS_FILENAME_MAXLEN 255

/* Inode number of the root inode */
const int SUFS_ROOT_INODE_NUMBER = 1;

struct sufs_dir_record {
	char filename[SUFS_FILENAME_MAXLEN];
	uint32_t inode_no;
};

struct sufs_inode {
	struct node *node;
	mode_t mode;
	uint32_t inode_no;
	uint32_t data_block_number;

	union {
		uint32_t file_size;
		uint32_t dir_children_count;
	};
};

/* Super block layout */
struct sufs_super_block {
	uint32_t version;
	uint32_t magic;
	uint32_t block_size;
	uint32_t free_blocks;
	char padding[SUFS_DEFAULT_BLOCK_SIZE - (4 * sizeof(uint32_t))
		     - sizeof(struct sufs_inode)];
	struct node root_node;
	struct sufs_inode root_inode;
};

