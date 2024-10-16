static int _regulator_do_set_voltage(struct regulator_dev *rdev,
				     int min_uV, int max_uV)
{
	int ret;
	int delay = 0;
	int best_val = 0;
	unsigned int selector;
	int old_selector = -1;

	trace_regulator_set_voltage(rdev_get_name(rdev), min_uV, max_uV);

	min_uV += rdev->constraints->uV_offset;
	max_uV += rdev->constraints->uV_offset;

	/*
	 * If we can't obtain the old selector there is not enough
	 * info to call set_voltage_time_sel().
	 */
	if (_regulator_is_enabled(rdev) &&
	    rdev->desc->ops->set_voltage_time_sel &&
	    rdev->desc->ops->get_voltage_sel) {
		old_selector = rdev->desc->ops->get_voltage_sel(rdev);
		if (old_selector < 0)
			return old_selector;
	}

	if (rdev->desc->ops->set_voltage) {
		ret = _regulator_call_set_voltage(rdev, min_uV, max_uV,
						  &selector);

		if (ret >= 0) {
			if (rdev->desc->ops->list_voltage)
				best_val = rdev->desc->ops->list_voltage(rdev,
									 selector);
			else
				best_val = _regulator_get_voltage(rdev);
		}

	} else if (rdev->desc->ops->set_voltage_sel) {
		if (rdev->desc->ops->map_voltage) {
			ret = rdev->desc->ops->map_voltage(rdev, min_uV,
							   max_uV);
		} else {
			if (rdev->desc->ops->list_voltage ==
			    regulator_list_voltage_linear)
				ret = regulator_map_voltage_linear(rdev,
								min_uV, max_uV);
			else if (rdev->desc->ops->list_voltage ==
				 regulator_list_voltage_linear_range)
				ret = regulator_map_voltage_linear_range(rdev,
								min_uV, max_uV);
			else
				ret = regulator_map_voltage_iterate(rdev,
								min_uV, max_uV);
		}

		if (ret >= 0) {
			best_val = rdev->desc->ops->list_voltage(rdev, ret);
			if (min_uV <= best_val && max_uV >= best_val) {
				selector = ret;
				if (old_selector == selector)
					ret = 0;
				else
					ret = _regulator_call_set_voltage_sel(
						rdev, best_val, selector);
			} else {
				ret = -EINVAL;
			}
		}
	} else {
		ret = -EINVAL;
	}

	/* Call set_voltage_time_sel if successfully obtained old_selector */
	if (ret == 0 && !rdev->constraints->ramp_disable && old_selector >= 0
		&& old_selector != selector) {

		delay = rdev->desc->ops->set_voltage_time_sel(rdev,
						old_selector, selector);
		if (delay < 0) {
			rdev_warn(rdev, "set_voltage_time_sel() failed: %d\n",
				  delay);
			delay = 0;
		}

		/* Insert any necessary delays */
		if (delay >= 1000) {
			mdelay(delay / 1000);
			udelay(delay % 1000);
		} else if (delay) {
			udelay(delay);
		}
	}

	if (ret == 0 && best_val >= 0) {
		unsigned long data = best_val;

		_notifier_call_chain(rdev, REGULATOR_EVENT_VOLTAGE_CHANGE,
				     (void *)data);
	}

	trace_regulator_set_voltage_complete(rdev_get_name(rdev), best_val);

	return ret;
}