static int regulator_check_consumers(struct regulator_dev *rdev,
				     int *min_uV, int *max_uV)
{
	struct regulator *regulator;

	list_for_each_entry(regulator, &rdev->consumer_list, list) {
		/*
		 * Assume consumers that didn't say anything are OK
		 * with anything in the constraint range.
		 */
		if (!regulator->min_uV && !regulator->max_uV)
			continue;

		if (*max_uV > regulator->max_uV)
			*max_uV = regulator->max_uV;
		if (*min_uV < regulator->min_uV)
			*min_uV = regulator->min_uV;
	}

	if (*min_uV > *max_uV) {
		rdev_err(rdev, "Restricting voltage, %u-%uuV\n",
			*min_uV, *max_uV);
		return -EINVAL;
	}

	return 0;
}