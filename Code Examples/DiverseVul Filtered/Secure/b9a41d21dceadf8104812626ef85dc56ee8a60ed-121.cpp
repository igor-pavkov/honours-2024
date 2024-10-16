static int dm_pr_preempt(struct block_device *bdev, u64 old_key, u64 new_key,
			 enum pr_type type, bool abort)
{
	struct mapped_device *md = bdev->bd_disk->private_data;
	const struct pr_ops *ops;
	fmode_t mode;
	int r;

	r = dm_grab_bdev_for_ioctl(md, &bdev, &mode);
	if (r < 0)
		return r;

	ops = bdev->bd_disk->fops->pr_ops;
	if (ops && ops->pr_preempt)
		r = ops->pr_preempt(bdev, old_key, new_key, type, abort);
	else
		r = -EOPNOTSUPP;

	bdput(bdev);
	return r;
}