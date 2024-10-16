void blk_unregister_region(dev_t devt, unsigned long range)
{
	kobj_unmap(bdev_map, devt, range);
}