int regulator_disable_deferred(struct regulator *regulator, int ms)
{
	struct regulator_dev *rdev = regulator->rdev;
	int ret;

	if (regulator->always_on)
		return 0;

	if (!ms)
		return regulator_disable(regulator);

	mutex_lock(&rdev->mutex);
	rdev->deferred_disables++;
	mutex_unlock(&rdev->mutex);

	ret = queue_delayed_work(system_power_efficient_wq,
				 &rdev->disable_work,
				 msecs_to_jiffies(ms));
	if (ret < 0)
		return ret;
	else
		return 0;
}