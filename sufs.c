/*
 * SuSF - The filesystem of a dystopian future
 *
 * Might do something, or does not
 *
 * See sufs.h for extensive documentation
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

#include "sufs.h"

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

int sufs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *inode;

	/* The incredible magic number */
	sb->s_magic = 0x5c1a31;

	inode = sufs_get_inode(sb, NULL, S_IFDIR, 0);
	sb->s_root = d_make_root(inode);
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}

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

static int sufs_init()
{
	int retval;

	retval = register_filesystem(&sufs_fs_type);
	if (unlikely(retval))
		pr_err("SuFS: Failed to register filesystem: %d\n", retval);
	else
		pr_info("SuFS: Module loaded\n");

	return 0;
}

static void sufs_exit()
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

