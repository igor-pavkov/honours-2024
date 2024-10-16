static int _regulator_do_enable(struct regulator_dev *rdev)
{
	int ret, delay;

	/* Query before enabling in case configuration dependent.  */
	ret = _regulator_get_enable_time(rdev);
	if (ret >= 0) {
		delay = ret;
	} else {
		rdev_warn(rdev, "enable_time() failed: %d\n", ret);
		delay = 0;
	}

	trace_regulator_enable(rdev_get_name(rdev));

	if (rdev->desc->off_on_delay) {
		/* if needed, keep a distance of off_on_delay from last time
		 * this regulator was disabled.
		 */
		unsigned long start_jiffy = jiffies;
		unsigned long intended, max_delay, remaining;

		max_delay = usecs_to_jiffies(rdev->desc->off_on_delay);
		intended = rdev->last_off_jiffy + max_delay;

		if (time_before(start_jiffy, intended)) {
			/* calc remaining jiffies to deal with one-time
			 * timer wrapping.
			 * in case of multiple timer wrapping, either it can be
			 * detected by out-of-range remaining, or it cannot be
			 * detected and we gets a panelty of
			 * _regulator_enable_delay().
			 */
			remaining = intended - start_jiffy;
			if (remaining <= max_delay)
				_regulator_enable_delay(
						jiffies_to_usecs(remaining));
		}
	}

	if (rdev->ena_pin) {
		ret = regulator_ena_gpio_ctrl(rdev, true);
		if (ret < 0)
			return ret;
		rdev->ena_gpio_state = 1;
	} else if (rdev->desc->ops->enable) {
		ret = rdev->desc->ops->enable(rdev);
		if (ret < 0)
			return ret;
	} else {
		return -EINVAL;
	}

	/* Allow the regulator to ramp; it would be useful to extend
	 * this for bulk operations so that the regulators can ramp
	 * together.  */
	trace_regulator_enable_delay(rdev_get_name(rdev));

	_regulator_enable_delay(delay);

	trace_regulator_enable_complete(rdev_get_name(rdev));

	return 0;
}