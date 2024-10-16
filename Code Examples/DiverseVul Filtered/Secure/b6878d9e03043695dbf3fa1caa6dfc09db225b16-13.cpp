static void sync_super(struct mddev *mddev, struct md_rdev *rdev)
{
	if (mddev->sync_super) {
		mddev->sync_super(mddev, rdev);
		return;
	}

	BUG_ON(mddev->major_version >= ARRAY_SIZE(super_types));

	super_types[mddev->major_version].sync_super(mddev, rdev);
}