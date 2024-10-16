static int dm_pr_reserve(struct block_device *bdev, u64 key, enum pr_type type,
			 u32 flags)
{
	struct mapped_device *md = bdev->bd_disk->private_data;
	const struct pr_ops *ops;
	fmode_t mode;
	int r;

	r = dm_grab_bdev_for_ioctl(md, &bdev, &mode);
	if (r < 0)
		return r;

	ops = bdev->bd_disk->fops->pr_ops;
	if (ops && ops->pr_reserve)
		r = ops->pr_reserve(bdev, key, type, flags);
	else
		r = -EOPNOTSUPP;

	bdput(bdev);
	return r;
}