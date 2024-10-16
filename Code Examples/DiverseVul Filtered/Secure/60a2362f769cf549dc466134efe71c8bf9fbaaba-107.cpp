int regulator_can_change_voltage(struct regulator *regulator)
{
	struct regulator_dev	*rdev = regulator->rdev;

	if (rdev->constraints &&
	    (rdev->constraints->valid_ops_mask & REGULATOR_CHANGE_VOLTAGE)) {
		if (rdev->desc->n_voltages - rdev->desc->linear_min_sel > 1)
			return 1;

		if (rdev->desc->continuous_voltage_range &&
		    rdev->constraints->min_uV && rdev->constraints->max_uV &&
		    rdev->constraints->min_uV != rdev->constraints->max_uV)
			return 1;
	}

	return 0;
}