static int __dm_pr_register(struct dm_target *ti, struct dm_dev *dev,
			    sector_t start, sector_t len, void *data)
{
	struct dm_pr *pr = data;
	const struct pr_ops *ops = dev->bdev->bd_disk->fops->pr_ops;

	if (!ops || !ops->pr_register)
		return -EOPNOTSUPP;
	return ops->pr_register(dev->bdev, pr->old_key, pr->new_key, pr->flags);
}