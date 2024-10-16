static int _regulator_get_voltage(struct regulator_dev *rdev)
{
	int sel, ret;

	if (rdev->desc->ops->get_voltage_sel) {
		sel = rdev->desc->ops->get_voltage_sel(rdev);
		if (sel < 0)
			return sel;
		ret = rdev->desc->ops->list_voltage(rdev, sel);
	} else if (rdev->desc->ops->get_voltage) {
		ret = rdev->desc->ops->get_voltage(rdev);
	} else if (rdev->desc->ops->list_voltage) {
		ret = rdev->desc->ops->list_voltage(rdev, 0);
	} else if (rdev->desc->fixed_uV && (rdev->desc->n_voltages == 1)) {
		ret = rdev->desc->fixed_uV;
	} else if (rdev->supply) {
		ret = regulator_get_voltage(rdev->supply);
	} else {
		return -EINVAL;
	}

	if (ret < 0)
		return ret;
	return ret - rdev->constraints->uV_offset;
}