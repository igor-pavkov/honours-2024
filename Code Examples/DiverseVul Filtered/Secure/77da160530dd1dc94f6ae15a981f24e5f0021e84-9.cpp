void device_add_disk(struct device *parent, struct gendisk *disk)
{
	struct backing_dev_info *bdi;
	dev_t devt;
	int retval;

	/* minors == 0 indicates to use ext devt from part0 and should
	 * be accompanied with EXT_DEVT flag.  Make sure all
	 * parameters make sense.
	 */
	WARN_ON(disk->minors && !(disk->major || disk->first_minor));
	WARN_ON(!disk->minors && !(disk->flags & GENHD_FL_EXT_DEVT));

	disk->flags |= GENHD_FL_UP;

	retval = blk_alloc_devt(&disk->part0, &devt);
	if (retval) {
		WARN_ON(1);
		return;
	}
	disk_to_dev(disk)->devt = devt;

	/* ->major and ->first_minor aren't supposed to be
	 * dereferenced from here on, but set them just in case.
	 */
	disk->major = MAJOR(devt);
	disk->first_minor = MINOR(devt);

	disk_alloc_events(disk);

	/* Register BDI before referencing it from bdev */
	bdi = &disk->queue->backing_dev_info;
	bdi_register_dev(bdi, disk_devt(disk));

	blk_register_region(disk_devt(disk), disk->minors, NULL,
			    exact_match, exact_lock, disk);
	register_disk(parent, disk);
	blk_register_queue(disk);

	/*
	 * Take an extra ref on queue which will be put on disk_release()
	 * so that it sticks around as long as @disk is there.
	 */
	WARN_ON_ONCE(!blk_get_queue(disk->queue));

	retval = sysfs_create_link(&disk_to_dev(disk)->kobj, &bdi->dev->kobj,
				   "bdi");
	WARN_ON(retval);

	disk_add_events(disk);
	blk_integrity_add(disk);
}