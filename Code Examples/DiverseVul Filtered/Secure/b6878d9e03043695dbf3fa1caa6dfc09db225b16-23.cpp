static int update_size(struct mddev *mddev, sector_t num_sectors)
{
	struct md_rdev *rdev;
	int rv;
	int fit = (num_sectors == 0);

	if (mddev->pers->resize == NULL)
		return -EINVAL;
	/* The "num_sectors" is the number of sectors of each device that
	 * is used.  This can only make sense for arrays with redundancy.
	 * linear and raid0 always use whatever space is available. We can only
	 * consider changing this number if no resync or reconstruction is
	 * happening, and if the new size is acceptable. It must fit before the
	 * sb_start or, if that is <data_offset, it must fit before the size
	 * of each device.  If num_sectors is zero, we find the largest size
	 * that fits.
	 */
	if (test_bit(MD_RECOVERY_RUNNING, &mddev->recovery) ||
	    mddev->sync_thread)
		return -EBUSY;
	if (mddev->ro)
		return -EROFS;

	rdev_for_each(rdev, mddev) {
		sector_t avail = rdev->sectors;

		if (fit && (num_sectors == 0 || num_sectors > avail))
			num_sectors = avail;
		if (avail < num_sectors)
			return -ENOSPC;
	}
	rv = mddev->pers->resize(mddev, num_sectors);
	if (!rv)
		revalidate_disk(mddev->gendisk);
	return rv;
}