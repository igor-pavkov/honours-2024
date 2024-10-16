int bdev_del_partition(struct gendisk *disk, int partno)
{
	struct block_device *part = NULL;
	int ret = -ENXIO;

	mutex_lock(&disk->open_mutex);
	part = xa_load(&disk->part_tbl, partno);
	if (!part)
		goto out_unlock;

	ret = -EBUSY;
	if (part->bd_openers)
		goto out_unlock;

	delete_partition(part);
	ret = 0;
out_unlock:
	mutex_unlock(&disk->open_mutex);
	return ret;
}