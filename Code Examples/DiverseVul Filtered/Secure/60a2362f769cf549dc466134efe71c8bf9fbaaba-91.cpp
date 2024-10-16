static int regulator_check_drms(struct regulator_dev *rdev)
{
	if (!rdev->constraints) {
		rdev_err(rdev, "no constraints\n");
		return -ENODEV;
	}
	if (!(rdev->constraints->valid_ops_mask & REGULATOR_CHANGE_DRMS)) {
		rdev_err(rdev, "operation not allowed\n");
		return -EPERM;
	}
	return 0;
}