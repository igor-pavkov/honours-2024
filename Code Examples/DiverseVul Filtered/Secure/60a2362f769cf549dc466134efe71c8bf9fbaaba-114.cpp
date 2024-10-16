int regulator_allow_bypass(struct regulator *regulator, bool enable)
{
	struct regulator_dev *rdev = regulator->rdev;
	int ret = 0;

	if (!rdev->desc->ops->set_bypass)
		return 0;

	if (rdev->constraints &&
	    !(rdev->constraints->valid_ops_mask & REGULATOR_CHANGE_BYPASS))
		return 0;

	mutex_lock(&rdev->mutex);

	if (enable && !regulator->bypass) {
		rdev->bypass_count++;

		if (rdev->bypass_count == rdev->open_count) {
			ret = rdev->desc->ops->set_bypass(rdev, enable);
			if (ret != 0)
				rdev->bypass_count--;
		}

	} else if (!enable && regulator->bypass) {
		rdev->bypass_count--;

		if (rdev->bypass_count != rdev->open_count) {
			ret = rdev->desc->ops->set_bypass(rdev, enable);
			if (ret != 0)
				rdev->bypass_count++;
		}
	}

	if (ret == 0)
		regulator->bypass = enable;

	mutex_unlock(&rdev->mutex);

	return ret;
}