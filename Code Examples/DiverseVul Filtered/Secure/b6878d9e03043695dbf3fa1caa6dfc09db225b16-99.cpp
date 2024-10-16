static struct md_rdev *find_rdev(struct mddev *mddev, dev_t dev)
{
	struct md_rdev *rdev;

	rdev_for_each(rdev, mddev)
		if (rdev->bdev->bd_dev == dev)
			return rdev;

	return NULL;
}