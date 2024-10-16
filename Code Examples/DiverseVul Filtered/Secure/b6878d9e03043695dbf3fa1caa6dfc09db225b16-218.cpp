void md_finish_reshape(struct mddev *mddev)
{
	/* called be personality module when reshape completes. */
	struct md_rdev *rdev;

	rdev_for_each(rdev, mddev) {
		if (rdev->data_offset > rdev->new_data_offset)
			rdev->sectors += rdev->data_offset - rdev->new_data_offset;
		else
			rdev->sectors -= rdev->new_data_offset - rdev->data_offset;
		rdev->data_offset = rdev->new_data_offset;
	}
}