static int _regulator_do_disable(struct regulator_dev *rdev)
{
	int ret;

	trace_regulator_disable(rdev_get_name(rdev));

	if (rdev->ena_pin) {
		ret = regulator_ena_gpio_ctrl(rdev, false);
		if (ret < 0)
			return ret;
		rdev->ena_gpio_state = 0;

	} else if (rdev->desc->ops->disable) {
		ret = rdev->desc->ops->disable(rdev);
		if (ret != 0)
			return ret;
	}

	/* cares about last_off_jiffy only if off_on_delay is required by
	 * device.
	 */
	if (rdev->desc->off_on_delay)
		rdev->last_off_jiffy = jiffies;

	trace_regulator_disable_complete(rdev_get_name(rdev));

	return 0;
}