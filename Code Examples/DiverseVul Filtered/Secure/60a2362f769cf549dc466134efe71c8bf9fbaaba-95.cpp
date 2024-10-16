static int __init regulator_init_complete(void)
{
	struct regulator_dev *rdev;
	const struct regulator_ops *ops;
	struct regulation_constraints *c;
	int enabled, ret;

	/*
	 * Since DT doesn't provide an idiomatic mechanism for
	 * enabling full constraints and since it's much more natural
	 * with DT to provide them just assume that a DT enabled
	 * system has full constraints.
	 */
	if (of_have_populated_dt())
		has_full_constraints = true;

	mutex_lock(&regulator_list_mutex);

	/* If we have a full configuration then disable any regulators
	 * we have permission to change the status for and which are
	 * not in use or always_on.  This is effectively the default
	 * for DT and ACPI as they have full constraints.
	 */
	list_for_each_entry(rdev, &regulator_list, list) {
		ops = rdev->desc->ops;
		c = rdev->constraints;

		if (c && c->always_on)
			continue;

		if (c && !(c->valid_ops_mask & REGULATOR_CHANGE_STATUS))
			continue;

		mutex_lock(&rdev->mutex);

		if (rdev->use_count)
			goto unlock;

		/* If we can't read the status assume it's on. */
		if (ops->is_enabled)
			enabled = ops->is_enabled(rdev);
		else
			enabled = 1;

		if (!enabled)
			goto unlock;

		if (have_full_constraints()) {
			/* We log since this may kill the system if it
			 * goes wrong. */
			rdev_info(rdev, "disabling\n");
			ret = _regulator_do_disable(rdev);
			if (ret != 0)
				rdev_err(rdev, "couldn't disable: %d\n", ret);
		} else {
			/* The intention is that in future we will
			 * assume that full constraints are provided
			 * so warn even if we aren't going to do
			 * anything here.
			 */
			rdev_warn(rdev, "incomplete constraints, leaving on\n");
		}

unlock:
		mutex_unlock(&rdev->mutex);
	}

	mutex_unlock(&regulator_list_mutex);

	return 0;
}