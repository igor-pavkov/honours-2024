static int add_regulator_attributes(struct regulator_dev *rdev)
{
	struct device *dev = &rdev->dev;
	const struct regulator_ops *ops = rdev->desc->ops;
	int status = 0;

	/* some attributes need specific methods to be displayed */
	if ((ops->get_voltage && ops->get_voltage(rdev) >= 0) ||
	    (ops->get_voltage_sel && ops->get_voltage_sel(rdev) >= 0) ||
	    (ops->list_voltage && ops->list_voltage(rdev, 0) >= 0) ||
		(rdev->desc->fixed_uV && (rdev->desc->n_voltages == 1))) {
		status = device_create_file(dev, &dev_attr_microvolts);
		if (status < 0)
			return status;
	}
	if (ops->get_current_limit) {
		status = device_create_file(dev, &dev_attr_microamps);
		if (status < 0)
			return status;
	}
	if (ops->get_mode) {
		status = device_create_file(dev, &dev_attr_opmode);
		if (status < 0)
			return status;
	}
	if (rdev->ena_pin || ops->is_enabled) {
		status = device_create_file(dev, &dev_attr_state);
		if (status < 0)
			return status;
	}
	if (ops->get_status) {
		status = device_create_file(dev, &dev_attr_status);
		if (status < 0)
			return status;
	}
	if (ops->get_bypass) {
		status = device_create_file(dev, &dev_attr_bypass);
		if (status < 0)
			return status;
	}

	/* some attributes are type-specific */
	if (rdev->desc->type == REGULATOR_CURRENT) {
		status = device_create_file(dev, &dev_attr_requested_microamps);
		if (status < 0)
			return status;
	}

	/* all the other attributes exist to support constraints;
	 * don't show them if there are no constraints, or if the
	 * relevant supporting methods are missing.
	 */
	if (!rdev->constraints)
		return status;

	/* constraints need specific supporting methods */
	if (ops->set_voltage || ops->set_voltage_sel) {
		status = device_create_file(dev, &dev_attr_min_microvolts);
		if (status < 0)
			return status;
		status = device_create_file(dev, &dev_attr_max_microvolts);
		if (status < 0)
			return status;
	}
	if (ops->set_current_limit) {
		status = device_create_file(dev, &dev_attr_min_microamps);
		if (status < 0)
			return status;
		status = device_create_file(dev, &dev_attr_max_microamps);
		if (status < 0)
			return status;
	}

	status = device_create_file(dev, &dev_attr_suspend_standby_state);
	if (status < 0)
		return status;
	status = device_create_file(dev, &dev_attr_suspend_mem_state);
	if (status < 0)
		return status;
	status = device_create_file(dev, &dev_attr_suspend_disk_state);
	if (status < 0)
		return status;

	if (ops->set_suspend_voltage) {
		status = device_create_file(dev,
				&dev_attr_suspend_standby_microvolts);
		if (status < 0)
			return status;
		status = device_create_file(dev,
				&dev_attr_suspend_mem_microvolts);
		if (status < 0)
			return status;
		status = device_create_file(dev,
				&dev_attr_suspend_disk_microvolts);
		if (status < 0)
			return status;
	}

	if (ops->set_suspend_mode) {
		status = device_create_file(dev,
				&dev_attr_suspend_standby_mode);
		if (status < 0)
			return status;
		status = device_create_file(dev,
				&dev_attr_suspend_mem_mode);
		if (status < 0)
			return status;
		status = device_create_file(dev,
				&dev_attr_suspend_disk_mode);
		if (status < 0)
			return status;
	}

	return status;
}