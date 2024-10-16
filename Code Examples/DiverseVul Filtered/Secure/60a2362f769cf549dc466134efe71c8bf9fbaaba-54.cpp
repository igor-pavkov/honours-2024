static int _regulator_call_set_voltage_sel(struct regulator_dev *rdev,
					   int uV, unsigned selector)
{
	struct pre_voltage_change_data data;
	int ret;

	data.old_uV = _regulator_get_voltage(rdev);
	data.min_uV = uV;
	data.max_uV = uV;
	ret = _notifier_call_chain(rdev, REGULATOR_EVENT_PRE_VOLTAGE_CHANGE,
				   &data);
	if (ret & NOTIFY_STOP_MASK)
		return -EINVAL;

	ret = rdev->desc->ops->set_voltage_sel(rdev, selector);
	if (ret >= 0)
		return ret;

	_notifier_call_chain(rdev, REGULATOR_EVENT_ABORT_VOLTAGE_CHANGE,
			     (void *)data.old_uV);

	return ret;
}