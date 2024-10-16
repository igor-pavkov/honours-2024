void set_device_ro(struct block_device *bdev, int flag)
{
	bdev->bd_part->policy = flag;
}