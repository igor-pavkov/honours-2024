int regulator_set_voltage(struct regulator *regulator, int min_uV, int max_uV)
{
	struct regulator_dev *rdev = regulator->rdev;
	int ret = 0;
	int old_min_uV, old_max_uV;
	int current_uV;

	mutex_lock(&rdev->mutex);

	/* If we're setting the same range as last time the change
	 * should be a noop (some cpufreq implementations use the same
	 * voltage for multiple frequencies, for example).
	 */
	if (regulator->min_uV == min_uV && regulator->max_uV == max_uV)
		goto out;

	/* If we're trying to set a range that overlaps the current voltage,
	 * return succesfully even though the regulator does not support
	 * changing the voltage.
	 */
	if (!(rdev->constraints->valid_ops_mask & REGULATOR_CHANGE_VOLTAGE)) {
		current_uV = _regulator_get_voltage(rdev);
		if (min_uV <= current_uV && current_uV <= max_uV) {
			regulator->min_uV = min_uV;
			regulator->max_uV = max_uV;
			goto out;
		}
	}

	/* sanity check */
	if (!rdev->desc->ops->set_voltage &&
	    !rdev->desc->ops->set_voltage_sel) {
		ret = -EINVAL;
		goto out;
	}

	/* constraints check */
	ret = regulator_check_voltage(rdev, &min_uV, &max_uV);
	if (ret < 0)
		goto out;

	/* restore original values in case of error */
	old_min_uV = regulator->min_uV;
	old_max_uV = regulator->max_uV;
	regulator->min_uV = min_uV;
	regulator->max_uV = max_uV;

	ret = regulator_check_consumers(rdev, &min_uV, &max_uV);
	if (ret < 0)
		goto out2;

	ret = _regulator_do_set_voltage(rdev, min_uV, max_uV);
	if (ret < 0)
		goto out2;

out:
	mutex_unlock(&rdev->mutex);
	return ret;
out2:
	regulator->min_uV = old_min_uV;
	regulator->max_uV = old_max_uV;
	mutex_unlock(&rdev->mutex);
	return ret;
}