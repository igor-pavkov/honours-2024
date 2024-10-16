int regulator_set_optimum_mode(struct regulator *regulator, int uA_load)
{
	struct regulator_dev *rdev = regulator->rdev;
	struct regulator *consumer;
	int ret, output_uV, input_uV = 0, total_uA_load = 0;
	unsigned int mode;

	if (rdev->supply)
		input_uV = regulator_get_voltage(rdev->supply);

	mutex_lock(&rdev->mutex);

	/*
	 * first check to see if we can set modes at all, otherwise just
	 * tell the consumer everything is OK.
	 */
	regulator->uA_load = uA_load;
	ret = regulator_check_drms(rdev);
	if (ret < 0) {
		ret = 0;
		goto out;
	}

	if (!rdev->desc->ops->get_optimum_mode)
		goto out;

	/*
	 * we can actually do this so any errors are indicators of
	 * potential real failure.
	 */
	ret = -EINVAL;

	if (!rdev->desc->ops->set_mode)
		goto out;

	/* get output voltage */
	output_uV = _regulator_get_voltage(rdev);
	if (output_uV <= 0) {
		rdev_err(rdev, "invalid output voltage found\n");
		goto out;
	}

	/* No supply? Use constraint voltage */
	if (input_uV <= 0)
		input_uV = rdev->constraints->input_uV;
	if (input_uV <= 0) {
		rdev_err(rdev, "invalid input voltage found\n");
		goto out;
	}

	/* calc total requested load for this regulator */
	list_for_each_entry(consumer, &rdev->consumer_list, list)
		total_uA_load += consumer->uA_load;

	mode = rdev->desc->ops->get_optimum_mode(rdev,
						 input_uV, output_uV,
						 total_uA_load);
	ret = regulator_mode_constrain(rdev, &mode);
	if (ret < 0) {
		rdev_err(rdev, "failed to get optimum mode @ %d uA %d -> %d uV\n",
			 total_uA_load, input_uV, output_uV);
		goto out;
	}

	ret = rdev->desc->ops->set_mode(rdev, mode);
	if (ret < 0) {
		rdev_err(rdev, "failed to set optimum mode %x\n", mode);
		goto out;
	}
	ret = mode;
out:
	mutex_unlock(&rdev->mutex);
	return ret;
}