static int blk_add_partitions(struct gendisk *disk)
{
	struct parsed_partitions *state;
	int ret = -EAGAIN, p;

	if (!disk_part_scan_enabled(disk))
		return 0;

	state = check_partition(disk);
	if (!state)
		return 0;
	if (IS_ERR(state)) {
		/*
		 * I/O error reading the partition table.  If we tried to read
		 * beyond EOD, retry after unlocking the native capacity.
		 */
		if (PTR_ERR(state) == -ENOSPC) {
			printk(KERN_WARNING "%s: partition table beyond EOD, ",
			       disk->disk_name);
			if (disk_unlock_native_capacity(disk))
				return -EAGAIN;
		}
		return -EIO;
	}

	/*
	 * Partitions are not supported on host managed zoned block devices.
	 */
	if (disk->queue->limits.zoned == BLK_ZONED_HM) {
		pr_warn("%s: ignoring partition table on host managed zoned block device\n",
			disk->disk_name);
		ret = 0;
		goto out_free_state;
	}

	/*
	 * If we read beyond EOD, try unlocking native capacity even if the
	 * partition table was successfully read as we could be missing some
	 * partitions.
	 */
	if (state->access_beyond_eod) {
		printk(KERN_WARNING
		       "%s: partition table partially beyond EOD, ",
		       disk->disk_name);
		if (disk_unlock_native_capacity(disk))
			goto out_free_state;
	}

	/* tell userspace that the media / partition table may have changed */
	kobject_uevent(&disk_to_dev(disk)->kobj, KOBJ_CHANGE);

	for (p = 1; p < state->limit; p++)
		if (!blk_add_partition(disk, state, p))
			goto out_free_state;

	ret = 0;
out_free_state:
	free_partitions(state);
	return ret;
}