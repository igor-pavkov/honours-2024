static void rdev_init_debugfs(struct regulator_dev *rdev)
{
	rdev->debugfs = debugfs_create_dir(rdev_get_name(rdev), debugfs_root);
	if (!rdev->debugfs) {
		rdev_warn(rdev, "Failed to create debugfs directory\n");
		return;
	}

	debugfs_create_u32("use_count", 0444, rdev->debugfs,
			   &rdev->use_count);
	debugfs_create_u32("open_count", 0444, rdev->debugfs,
			   &rdev->open_count);
	debugfs_create_u32("bypass_count", 0444, rdev->debugfs,
			   &rdev->bypass_count);
}