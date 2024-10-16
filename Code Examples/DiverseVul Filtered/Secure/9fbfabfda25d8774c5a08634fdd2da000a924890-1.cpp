int bdev_disk_changed(struct gendisk *disk, bool invalidate)
{
	int ret = 0;

	lockdep_assert_held(&disk->open_mutex);

	if (!disk_live(disk))
		return -ENXIO;

rescan:
	if (disk->open_partitions)
		return -EBUSY;
	sync_blockdev(disk->part0);
	invalidate_bdev(disk->part0);
	blk_drop_partitions(disk);

	clear_bit(GD_NEED_PART_SCAN, &disk->state);

	/*
	 * Historically we only set the capacity to zero for devices that
	 * support partitions (independ of actually having partitions created).
	 * Doing that is rather inconsistent, but changing it broke legacy
	 * udisks polling for legacy ide-cdrom devices.  Use the crude check
	 * below to get the sane behavior for most device while not breaking
	 * userspace for this particular setup.
	 */
	if (invalidate) {
		if (disk_part_scan_enabled(disk) ||
		    !(disk->flags & GENHD_FL_REMOVABLE))
			set_capacity(disk, 0);
	}

	if (get_capacity(disk)) {
		ret = blk_add_partitions(disk);
		if (ret == -EAGAIN)
			goto rescan;
	} else if (invalidate) {
		/*
		 * Tell userspace that the media / partition table may have
		 * changed.
		 */
		kobject_uevent(&disk_to_dev(disk)->kobj, KOBJ_CHANGE);
	}

	return ret;
}