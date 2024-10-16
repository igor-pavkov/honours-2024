static void print_constraints(struct regulator_dev *rdev)
{
	struct regulation_constraints *constraints = rdev->constraints;
	char buf[80] = "";
	int count = 0;
	int ret;

	if (constraints->min_uV && constraints->max_uV) {
		if (constraints->min_uV == constraints->max_uV)
			count += sprintf(buf + count, "%d mV ",
					 constraints->min_uV / 1000);
		else
			count += sprintf(buf + count, "%d <--> %d mV ",
					 constraints->min_uV / 1000,
					 constraints->max_uV / 1000);
	}

	if (!constraints->min_uV ||
	    constraints->min_uV != constraints->max_uV) {
		ret = _regulator_get_voltage(rdev);
		if (ret > 0)
			count += sprintf(buf + count, "at %d mV ", ret / 1000);
	}

	if (constraints->uV_offset)
		count += sprintf(buf, "%dmV offset ",
				 constraints->uV_offset / 1000);

	if (constraints->min_uA && constraints->max_uA) {
		if (constraints->min_uA == constraints->max_uA)
			count += sprintf(buf + count, "%d mA ",
					 constraints->min_uA / 1000);
		else
			count += sprintf(buf + count, "%d <--> %d mA ",
					 constraints->min_uA / 1000,
					 constraints->max_uA / 1000);
	}

	if (!constraints->min_uA ||
	    constraints->min_uA != constraints->max_uA) {
		ret = _regulator_get_current_limit(rdev);
		if (ret > 0)
			count += sprintf(buf + count, "at %d mA ", ret / 1000);
	}

	if (constraints->valid_modes_mask & REGULATOR_MODE_FAST)
		count += sprintf(buf + count, "fast ");
	if (constraints->valid_modes_mask & REGULATOR_MODE_NORMAL)
		count += sprintf(buf + count, "normal ");
	if (constraints->valid_modes_mask & REGULATOR_MODE_IDLE)
		count += sprintf(buf + count, "idle ");
	if (constraints->valid_modes_mask & REGULATOR_MODE_STANDBY)
		count += sprintf(buf + count, "standby");

	if (!count)
		sprintf(buf, "no parameters");

	rdev_dbg(rdev, "%s\n", buf);

	if ((constraints->min_uV != constraints->max_uV) &&
	    !(constraints->valid_ops_mask & REGULATOR_CHANGE_VOLTAGE))
		rdev_warn(rdev,
			  "Voltage range but no REGULATOR_CHANGE_VOLTAGE\n");
}