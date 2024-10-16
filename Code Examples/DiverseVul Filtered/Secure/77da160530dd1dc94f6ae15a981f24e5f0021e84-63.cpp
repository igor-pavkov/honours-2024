static int __init genhd_device_init(void)
{
	int error;

	block_class.dev_kobj = sysfs_dev_block_kobj;
	error = class_register(&block_class);
	if (unlikely(error))
		return error;
	bdev_map = kobj_map_init(base_probe, &block_class_lock);
	blk_dev_init();

	register_blkdev(BLOCK_EXT_MAJOR, "blkext");

	/* create top-level block dir */
	if (!sysfs_deprecated)
		block_depr = kobject_create_and_add("block", NULL);
	return 0;
}