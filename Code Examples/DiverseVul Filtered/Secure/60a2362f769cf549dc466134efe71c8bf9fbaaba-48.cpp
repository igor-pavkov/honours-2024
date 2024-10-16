int regulator_suspend_finish(void)
{
	struct regulator_dev *rdev;
	int ret = 0, error;

	mutex_lock(&regulator_list_mutex);
	list_for_each_entry(rdev, &regulator_list, list) {
		mutex_lock(&rdev->mutex);
		if (rdev->use_count > 0  || rdev->constraints->always_on) {
			error = _regulator_do_enable(rdev);
			if (error)
				ret = error;
		} else {
			if (!have_full_constraints())
				goto unlock;
			if (!_regulator_is_enabled(rdev))
				goto unlock;

			error = _regulator_do_disable(rdev);
			if (error)
				ret = error;
		}
unlock:
		mutex_unlock(&rdev->mutex);
	}
	mutex_unlock(&regulator_list_mutex);
	return ret;
}