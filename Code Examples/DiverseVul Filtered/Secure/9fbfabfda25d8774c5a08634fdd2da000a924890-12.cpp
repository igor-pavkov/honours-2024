int bdev_add_partition(struct gendisk *disk, int partno, sector_t start,
		sector_t length)
{
	struct block_device *part;
	int ret;

	mutex_lock(&disk->open_mutex);
	if (!disk_live(disk)) {
		ret = -ENXIO;
		goto out;
	}

	if (partition_overlaps(disk, start, length, -1)) {
		ret = -EBUSY;
		goto out;
	}

	part = add_partition(disk, partno, start, length,
			ADDPART_FLAG_NONE, NULL);
	ret = PTR_ERR_OR_ZERO(part);
out:
	mutex_unlock(&disk->open_mutex);
	return ret;
}