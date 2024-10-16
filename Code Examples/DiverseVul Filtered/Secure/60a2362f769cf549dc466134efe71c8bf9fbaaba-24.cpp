int rdev_get_id(struct regulator_dev *rdev)
{
	return rdev->desc->id;
}