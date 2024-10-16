static bool disk_unlock_native_capacity(struct gendisk *disk)
{
	const struct block_device_operations *bdops = disk->fops;

	if (bdops->unlock_native_capacity &&
	    !(disk->flags & GENHD_FL_NATIVE_CAPACITY)) {
		printk(KERN_CONT "enabling native capacity\n");
		bdops->unlock_native_capacity(disk);
		disk->flags |= GENHD_FL_NATIVE_CAPACITY;
		return true;
	} else {
		printk(KERN_CONT "truncated\n");
		return false;
	}
}