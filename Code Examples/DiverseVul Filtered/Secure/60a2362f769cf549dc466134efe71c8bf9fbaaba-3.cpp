void *rdev_get_drvdata(struct regulator_dev *rdev)
{
	return rdev->reg_data;
}