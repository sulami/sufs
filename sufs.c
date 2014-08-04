/*
 * SuSF - The filesystem of a dystopian future
 *
 * Might do something, or does not
 */

#include <linux/init.h>
#include <linux/module.h>

#include "sufs.h"

static int sufs_init()
{
	pr_info("SuFS loaded\n");
	return 0;
}

static void sufs_exit()
{
	pr_info("SuFS unloaded\n");
}

module_init(sufs_init);
module_exit(sufs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robin Schroer");
MODULE_DESCRIPTION("SuFS out-of-tree module");

