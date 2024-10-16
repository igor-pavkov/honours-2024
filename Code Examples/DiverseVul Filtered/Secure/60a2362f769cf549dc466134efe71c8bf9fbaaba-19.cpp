int regulator_list_voltage(struct regulator *regulator, unsigned selector)
{
	struct regulator_dev *rdev = regulator->rdev;
	const struct regulator_ops *ops = rdev->desc->ops;
	int ret;

	if (rdev->desc->fixed_uV && rdev->desc->n_voltages == 1 && !selector)
		return rdev->desc->fixed_uV;

	if (ops->list_voltage) {
		if (selector >= rdev->desc->n_voltages)
			return -EINVAL;
		mutex_lock(&rdev->mutex);
		ret = ops->list_voltage(rdev, selector);
		mutex_unlock(&rdev->mutex);
	} else if (rdev->supply) {
		ret = regulator_list_voltage(rdev->supply, selector);
	} else {
		return -EINVAL;
	}

	if (ret > 0) {
		if (ret < rdev->constraints->min_uV)
			ret = 0;
		else if (ret > rdev->constraints->max_uV)
			ret = 0;
	}

	return ret;
}