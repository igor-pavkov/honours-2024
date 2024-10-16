void *regulator_get_drvdata(struct regulator *regulator)
{
	return regulator->rdev->reg_data;
}