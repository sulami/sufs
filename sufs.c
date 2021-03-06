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
 * Debug print macros
 */
#define spr(level, fmt, ...)				\
	printk(level "SuFS: " fmt "\n", ##__VA_ARGS__)	\

/*
 * Locking
 */
static DEFINE_MUTEX(sufs_file_lock);

/*
 * sufs_iterate
 *
 * Implement the iterate file operation
 */
static int sufs_iterate(struct file *filp, struct dir_context *ctx)
{
	return 0;
}

const struct file_operations sufs_dir_operations = {
	.owner = THIS_MODULE,
	.iterate = sufs_iterate,
};

struct dentry *sufs_lookup(struct inode *parent_inode,
			   struct dentry *child_dentry, unsigned int flags);
static int sufs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
		       bool excl);
static int sufs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode);

static struct inode_operations sufs_inode_ops = {
	.create = sufs_create,
	.lookup = sufs_lookup,
	.mkdir = sufs_mkdir,
};

/*
 * sufs_create_fs_object
 *
 * Create a filesystem object, to be called by the functions in sufs_inode_ops
 */
static int sufs_create_fs_object(struct inode *dir, struct dentry *dentry,
				 umode_t mode)
{
	mutex_lock_interruptible(&sufs_file_lock);
	mutex_unlock(&sufs_file_lock);
	return 0;
}

/*
 * sufs_lookup
 *
 * Implement the lookup inode operation
 */
struct dentry *sufs_lookup(struct inode *parent_inode,
			   struct dentry *child_dentry, unsigned int flags)
{
	return NULL;
}

/*
 * sufs_create
 *
 * Implement the create inode operation
 */
static int sufs_create(struct inode *dir, struct dentry *dentry, umode_t mode,
		       bool excl)
{
	return sufs_create_fs_object(dir, dentry, mode);
}

/*
 * sufs_mkdir
 *
 * Implement the mkdir inode operation
 */
static int sufs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	return sufs_create_fs_object(dir, dentry, S_IFDIR | mode);
}

/*
 * sufs_get_inode
 *
 * Create a new inode and return it for a given file (or directory, link, ...)
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
		spr(KERN_ERR, "This type of inode is not supported yet");
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
 */
int sufs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root_inode;
	struct buffer_head *bh;
	struct sufs_super_block *sb_disk;

	bh = (struct buffer_head *)sb_bread(sb, 0);
	sb_disk = (struct sufs_super_block *)bh->b_data;

	if (!silent)
		spr(KERN_INFO, "Magic number on disk is %d", sb_disk->magic);

	if (unlikely(sb_disk->magic != SUFS_MAGIC_NR)) {
		spr(KERN_ERR, "SuFS: Filesystem does not seem to be SuFS");
		return -EMEDIUMTYPE;
	}

	if (unlikely(sb_disk->block_size != SUFS_DEFAULT_BLOCK_SIZE)) {
		spr(KERN_ERR,
		    "Filesystem seems to use non-standard block size");
		return -EMEDIUMTYPE;
	}

	if (!silent)
		spr(KERN_INFO, "Filesystem detected, version %d, block size %d",
		    sb_disk->version, sb_disk->block_size);

	sb->s_magic = SUFS_MAGIC_NR;

	root_inode = new_indoe(sb);
	root_inode->i_ino = SUFS_ROOT_INODE_NUMBER;
	inode_init_owner(root_inode, NULL, S_IFDIR);
	root_inode->i_sb = sb;
	root_inode->i_op = &sufs_inode_ops;
	root_inode->i_fop = &sufs_dir_operations;
	root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime
			    = CURRENT_TIME;
	root_inode->i_private = &(sb_disk->root_inode);

	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}

/*
 * sufs_mount
 *
 * Mount a SuFS filesystem
 */
static struct dentry *sufs_mount(struct file_system_type *fs_type, int flags,
				 const char *dev_name, void *data)
{
	struct dentry *retval;

	retval = mount_bdev(fs_type, flags, dev_name, data, sufs_fill_super);
	if (unlikely(IS_ERR(retval)))
		spr(KERN_ERR, "Error mounting filesystem");
	else
		spr(KERN_INFO, "Mount successful on %s", dev_name);

	return retval;
}

/*
 * sufs_kill_superblock
 *
 * Kill the superblock and unmount the filesystem. Currently a dummy
 */
static void sufs_kill_superblock(struct super_block *sb)
{
	spr(KERN_INFO, "Superblock is dead, unmount successful");
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
		spr(KERN_ERR, "Failed to register filesystem: %d", retval);
	else
		spr(KERN_INFO, "Module loaded");

	return 0;
}

static void sufs_exit(void)
{
	int retval;

	retval = unregister_filesystem(&sufs_fs_type);
	if (unlikely(retval))
		spr(KERN_ERR, "Failed to unregister filesystem: %d", retval);
	else
		spr(KERN_INFO, "Module unloaded");
}

module_init(sufs_init);
module_exit(sufs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robin Schroer");
MODULE_DESCRIPTION("SuFS out-of-tree module");

