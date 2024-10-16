int bdev_resize_partition(struct gendisk *disk, int partno, sector_t start,
		sector_t length)
{
	struct block_device *part = NULL;
	int ret = -ENXIO;

	mutex_lock(&disk->open_mutex);
	part = xa_load(&disk->part_tbl, partno);
	if (!part)
		goto out_unlock;

	ret = -EINVAL;
	if (start != part->bd_start_sect)
		goto out_unlock;

	ret = -EBUSY;
	if (partition_overlaps(disk, start, length, partno))
		goto out_unlock;

	bdev_set_nr_sectors(part, length);

	ret = 0;
out_unlock:
	mutex_unlock(&disk->open_mutex);
	return ret;
}