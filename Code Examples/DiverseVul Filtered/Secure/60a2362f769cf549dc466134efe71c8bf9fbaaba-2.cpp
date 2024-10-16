int regulator_list_hardware_vsel(struct regulator *regulator,
				 unsigned selector)
{
	struct regulator_dev *rdev = regulator->rdev;
	const struct regulator_ops *ops = rdev->desc->ops;

	if (selector >= rdev->desc->n_voltages)
		return -EINVAL;
	if (ops->set_voltage_sel != regulator_set_voltage_sel_regmap)
		return -EOPNOTSUPP;

	return selector;
}