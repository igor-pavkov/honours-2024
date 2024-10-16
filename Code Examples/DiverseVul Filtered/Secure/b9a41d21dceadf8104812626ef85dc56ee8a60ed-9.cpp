static int dm_pr_release(struct block_device *bdev, u64 key, enum pr_type type)
{
	struct mapped_device *md = bdev->bd_disk->private_data;
	const struct pr_ops *ops;
	fmode_t mode;
	int r;

	r = dm_grab_bdev_for_ioctl(md, &bdev, &mode);
	if (r < 0)
		return r;

	ops = bdev->bd_disk->fops->pr_ops;
	if (ops && ops->pr_release)
		r = ops->pr_release(bdev, key, type);
	else
		r = -EOPNOTSUPP;

	bdput(bdev);
	return r;
}