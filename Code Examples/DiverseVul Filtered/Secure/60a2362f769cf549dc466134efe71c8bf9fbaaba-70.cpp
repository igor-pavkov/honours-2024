static int regulator_ena_gpio_ctrl(struct regulator_dev *rdev, bool enable)
{
	struct regulator_enable_gpio *pin = rdev->ena_pin;

	if (!pin)
		return -EINVAL;

	if (enable) {
		/* Enable GPIO at initial use */
		if (pin->enable_count == 0)
			gpiod_set_value_cansleep(pin->gpiod,
						 !pin->ena_gpio_invert);

		pin->enable_count++;
	} else {
		if (pin->enable_count > 1) {
			pin->enable_count--;
			return 0;
		}

		/* Disable GPIO if not used */
		if (pin->enable_count <= 1) {
			gpiod_set_value_cansleep(pin->gpiod,
						 pin->ena_gpio_invert);
			pin->enable_count = 0;
		}
	}

	return 0;
}