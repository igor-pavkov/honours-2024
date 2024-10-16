unsigned int regulator_get_linear_step(struct regulator *regulator)
{
	struct regulator_dev *rdev = regulator->rdev;

	return rdev->desc->uV_step;
}