static int _regulator_get_enable_time(struct regulator_dev *rdev)
{
	if (rdev->constraints && rdev->constraints->enable_time)
		return rdev->constraints->enable_time;
	if (!rdev->desc->ops->enable_time)
		return rdev->desc->enable_time;
	return rdev->desc->ops->enable_time(rdev);
}