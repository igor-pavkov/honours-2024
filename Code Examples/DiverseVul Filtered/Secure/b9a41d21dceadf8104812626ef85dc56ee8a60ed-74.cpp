static int dm_blk_ioctl(struct block_device *bdev, fmode_t mode,
			unsigned int cmd, unsigned long arg)
{
	struct mapped_device *md = bdev->bd_disk->private_data;
	int r;

	r = dm_grab_bdev_for_ioctl(md, &bdev, &mode);
	if (r < 0)
		return r;

	if (r > 0) {
		/*
		 * Target determined this ioctl is being issued against a
		 * subset of the parent bdev; require extra privileges.
		 */
		if (!capable(CAP_SYS_RAWIO)) {
			DMWARN_LIMIT(
	"%s: sending ioctl %x to DM device without required privilege.",
				current->comm, cmd);
			r = -ENOIOCTLCMD;
			goto out;
		}
	}

	r =  __blkdev_driver_ioctl(bdev, mode, cmd, arg);
out:
	bdput(bdev);
	return r;
}