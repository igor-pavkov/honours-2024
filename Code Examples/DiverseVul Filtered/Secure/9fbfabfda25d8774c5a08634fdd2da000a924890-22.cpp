static bool blk_add_partition(struct gendisk *disk,
		struct parsed_partitions *state, int p)
{
	sector_t size = state->parts[p].size;
	sector_t from = state->parts[p].from;
	struct block_device *part;

	if (!size)
		return true;

	if (from >= get_capacity(disk)) {
		printk(KERN_WARNING
		       "%s: p%d start %llu is beyond EOD, ",
		       disk->disk_name, p, (unsigned long long) from);
		if (disk_unlock_native_capacity(disk))
			return false;
		return true;
	}

	if (from + size > get_capacity(disk)) {
		printk(KERN_WARNING
		       "%s: p%d size %llu extends beyond EOD, ",
		       disk->disk_name, p, (unsigned long long) size);

		if (disk_unlock_native_capacity(disk))
			return false;

		/*
		 * We can not ignore partitions of broken tables created by for
		 * example camera firmware, but we limit them to the end of the
		 * disk to avoid creating invalid block devices.
		 */
		size = get_capacity(disk) - from;
	}

	part = add_partition(disk, p, from, size, state->parts[p].flags,
			     &state->parts[p].info);
	if (IS_ERR(part) && PTR_ERR(part) != -ENXIO) {
		printk(KERN_ERR " %s: p%d could not be added: %ld\n",
		       disk->disk_name, p, -PTR_ERR(part));
		return true;
	}

	if (IS_BUILTIN(CONFIG_BLK_DEV_MD) &&
	    (state->parts[p].flags & ADDPART_FLAG_RAID))
		md_autodetect_dev(part->bd_dev);

	return true;
}