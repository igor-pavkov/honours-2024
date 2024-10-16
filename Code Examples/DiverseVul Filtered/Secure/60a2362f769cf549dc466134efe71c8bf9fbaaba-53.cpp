static int _regulator_call_set_voltage(struct regulator_dev *rdev,
				       int min_uV, int max_uV,
				       unsigned *selector)
{
	struct pre_voltage_change_data data;
	int ret;

	data.old_uV = _regulator_get_voltage(rdev);
	data.min_uV = min_uV;
	data.max_uV = max_uV;
	ret = _notifier_call_chain(rdev, REGULATOR_EVENT_PRE_VOLTAGE_CHANGE,
				   &data);
	if (ret & NOTIFY_STOP_MASK)
		return -EINVAL;

	ret = rdev->desc->ops->set_voltage(rdev, min_uV, max_uV, selector);
	if (ret >= 0)
		return ret;

	_notifier_call_chain(rdev, REGULATOR_EVENT_ABORT_VOLTAGE_CHANGE,
			     (void *)data.old_uV);

	return ret;
}