int bdev_read_only(struct block_device *bdev)
{
	if (!bdev)
		return 0;
	return bdev->bd_part->policy;
}