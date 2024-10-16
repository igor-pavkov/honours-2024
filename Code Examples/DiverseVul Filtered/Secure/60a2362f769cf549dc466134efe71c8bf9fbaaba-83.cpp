static int _regulator_force_disable(struct regulator_dev *rdev)
{
	int ret = 0;

	ret = _regulator_do_disable(rdev);
	if (ret < 0) {
		rdev_err(rdev, "failed to force disable\n");
		return ret;
	}

	_notifier_call_chain(rdev, REGULATOR_EVENT_FORCE_DISABLE |
			REGULATOR_EVENT_DISABLE, NULL);

	return 0;
}