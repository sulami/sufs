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

struct file_system_type sufs_fs_type = {
	.owner = THIS_MODULE,
	.name = "sufs",
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

