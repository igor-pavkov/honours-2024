static struct regulator *_regulator_get(struct device *dev, const char *id,
					bool exclusive, bool allow_dummy)
{
	struct regulator_dev *rdev;
	struct regulator *regulator = ERR_PTR(-EPROBE_DEFER);
	const char *devname = NULL;
	int ret;

	if (id == NULL) {
		pr_err("get() with no identifier\n");
		return ERR_PTR(-EINVAL);
	}

	if (dev)
		devname = dev_name(dev);

	if (have_full_constraints())
		ret = -ENODEV;
	else
		ret = -EPROBE_DEFER;

	mutex_lock(&regulator_list_mutex);

	rdev = regulator_dev_lookup(dev, id, &ret);
	if (rdev)
		goto found;

	regulator = ERR_PTR(ret);

	/*
	 * If we have return value from dev_lookup fail, we do not expect to
	 * succeed, so, quit with appropriate error value
	 */
	if (ret && ret != -ENODEV)
		goto out;

	if (!devname)
		devname = "deviceless";

	/*
	 * Assume that a regulator is physically present and enabled
	 * even if it isn't hooked up and just provide a dummy.
	 */
	if (have_full_constraints() && allow_dummy) {
		pr_warn("%s supply %s not found, using dummy regulator\n",
			devname, id);

		rdev = dummy_regulator_rdev;
		goto found;
	/* Don't log an error when called from regulator_get_optional() */
	} else if (!have_full_constraints() || exclusive) {
		dev_warn(dev, "dummy supplies not allowed\n");
	}

	mutex_unlock(&regulator_list_mutex);
	return regulator;

found:
	if (rdev->exclusive) {
		regulator = ERR_PTR(-EPERM);
		goto out;
	}

	if (exclusive && rdev->open_count) {
		regulator = ERR_PTR(-EBUSY);
		goto out;
	}

	if (!try_module_get(rdev->owner))
		goto out;

	regulator = create_regulator(rdev, dev, id);
	if (regulator == NULL) {
		regulator = ERR_PTR(-ENOMEM);
		module_put(rdev->owner);
		goto out;
	}

	rdev->open_count++;
	if (exclusive) {
		rdev->exclusive = 1;

		ret = _regulator_is_enabled(rdev);
		if (ret > 0)
			rdev->use_count = 1;
		else
			rdev->use_count = 0;
	}

out:
	mutex_unlock(&regulator_list_mutex);

	return regulator;
}