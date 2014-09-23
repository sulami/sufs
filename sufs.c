/*
 * SuSF - The filesystem of a dystopian future
 *
 * Might do something, might not
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#include "sufs.h"

/*
 * sufs_iterate
 *
 * Implement the iterate file operation
 *
 * TAKES:
 * struct file *filp		filepointer to use
 * struct dir_context *ctx	context to use
 */
static int sufs_iterate(struct file *filp, struct dir_context *ctx)
{
	return 0;
}

const struct file_operations sufs_dir_operations = {
	.owner = THIS_MODULE,
	.iterate = sufs_iterate,
};

/*
 * sufs_lookup
 *
 * Implement the lookup inode operation
 *
 * TAKES:
 * struct inode *parent_inode	the parent inode
 * struct dentry *child_dentry	the child dentry
 * unsigned int flags		additional flags
 */
struct dentry *sufs_lookup(struct inode *parent_inode,
			   struct dentry *child_dentry, unsigned int flags)
{
	return NULL;
}

static struct inode_operations sufs_inode_ops = {
	.lookup = sufs_lookup,
};

/*
 * sufs_get_inode
 *
 * Create a new inode and return it for a given file (or directory, link, ...)
 *
 * TAKES:
 * struct super_block *sb	superblock which manages the file
 * const struct indoe *dir	directory under which the file is created
 * umode_t mode			type of file
 * dev_t dev			device to be used
 */
struct inode *sufs_get_inode(struct super_block *sb, const struct inode *dir,
			     umode_t mode, dev_t dev)
{
	struct inode *inode = new_inode(sb);
	if (!inode)
		goto fail;

	inode->i_ino = get_next_ino();
	inode_init_owner(inode, dir, mode);
	inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;

	switch (mode & S_IFMT) {
	case S_IFDIR:
		inc_nlink(inode);
		break;
	case S_IFREG:
	case S_IFLNK:
	default:
		pr_err("SuFS: This type of inode is not supported yet\n");
		free_inode_nonrcu(inode);
		break;
	}

fail:
	return inode;
}

/*
 * syfs_fill_super
 *
 * Fill the superblock with the information needed to be valid
 *
 * TAKES:
 * struct super_block *sb	superblock to fill
 * void *data			data?
 * int silent			silent?
 */
int sufs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;
	struct buffer_head *bh;
	struct sufs_super_block *sb_disk;

	bh = (struct buffer_head *)sb_bread(sb, 0);
	sb_disk = (struct sufs_super_block *)bh->b_data;

	if (!silent)
		pr_info("SuFS: Magic number on disk is %d\n", sb_disk->magic);

	if (unlikely(sb_disk->magic != SUFS_MAGIC_NR)) {
		pr_err("SuFS: Filesystem does not seem to be SuFS\n");
		return -EMEDIUMTYPE;
	}

	if (unlikely(sb_disk->block_size != SUFS_DEFAULT_BLOCK_SIZE)) {
		pr_err(
		"SuFS: Filesystem seems to use non-standard block size\n");
		return -EMEDIUMTYPE;
	}

	if (!silent)
		pr_info("SuFS: Filesystem detected, version %d, block size %d\n",
			sb_disk->version, sb_disk->block_size);

	sb->s_magic = SUFS_MAGIC_NR;
	inode = sufs_get_inode(sb, NULL, S_IFDIR, 0);
	inode->i_op = &sufs_inode_ops;
	inode->i_fop = &sufs_dir_operations;
	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}

/*
 * sufs_mount
 *
 * Mount a SuFS filesystem
 *
 * TAKES:
 * struct file_system_type *fs_type	the filesystem tupe
 * int flags				additional flags
 * const_char *dev_name			the device name
 * void *data				data?
 */
static struct dentry *sufs_mount(struct file_system_type *fs_type, int flags,
				 const char *dev_name, void *data)
{
	struct dentry *retval;

	retval = mount_bdev(fs_type, flags, dev_name, data, sufs_fill_super);
	if (unlikely(IS_ERR(retval)))
		pr_err("SuFS: Error mounting filesystem\n");
	else
		pr_info("SuFS: Mount successful on %s\n", dev_name);

	return retval;
}

/*
 * sufs_kill_superblock
 *
 * Kill the superblock and unmount the filesystem. Currently a dummy
 *
 * TAKES:
 * struct super_block *sb	super block to kill
 */
static void sufs_kill_superblock(struct super_block *sb)
{
	pr_info("SuFS: Superblock is dead, unmount successful\n");
}

struct file_system_type sufs_fs_type = {
	.owner = THIS_MODULE,
	.name = "sufs",
	.mount = sufs_mount,
	.kill_sb = sufs_kill_superblock,
};

static int sufs_init(void)
{
	int retval;

	retval = register_filesystem(&sufs_fs_type);
	if (unlikely(retval))
		pr_err("SuFS: Failed to register filesystem: %d\n", retval);
	else
		pr_info("SuFS: Module loaded\n");

	return 0;
}

static void sufs_exit(void)
{
	int retval;

	retval = unregister_filesystem(&sufs_fs_type);
	if (unlikely(retval))
		pr_err("SuFS: Failed to unregister filesystem: %d\n", retval);
	else
		pr_info("SuFS: Module unloaded\n");
}

module_init(sufs_init);
module_exit(sufs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robin Schroer");
MODULE_DESCRIPTION("SuFS out-of-tree module");

