int regulator_count_voltages(struct regulator *regulator)
{
	struct regulator_dev	*rdev = regulator->rdev;

	if (rdev->desc->n_voltages)
		return rdev->desc->n_voltages;

	if (!rdev->supply)
		return -EINVAL;

	return regulator_count_voltages(rdev->supply);
}