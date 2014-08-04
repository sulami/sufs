/*
 * SuFS Header - Declarations and documentation
 */

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
			     umode_t mode, dev_t dev);

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
int sufs_fill_super(struct super_block *sb, void *data, int silent);

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
				 const char *dev_name, void *data);

/*
 * sufs_kill_superblock
 *
 * Kill the superblock and unmount the filesystem. Currently a dummy
 *
 * TAKES:
 * struct super_block *sb	super block to kill
 */
static void sufs_kill_superblock(struct super_block *sb);

/*
 * sufs_init
 *
 * Generic kernel module init function. Does nothing right now except
 * printing out an info message
 */
static int sufs_init(void);

/*
 * sufs_exit
 *
 * Generic kernel module exit function. Does nothing right now except
 * printing out and info message
 */
static void sufs_exit(void);

