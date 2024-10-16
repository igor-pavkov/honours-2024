struct device *rdev_get_dev(struct regulator_dev *rdev)
{
	return &rdev->dev;
}