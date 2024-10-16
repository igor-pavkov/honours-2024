int em28xx_tuner_callback(void *ptr, int component, int command, int arg)
{
	struct em28xx_i2c_bus *i2c_bus = ptr;
	struct em28xx *dev = i2c_bus->dev;
	int rc = 0;

	if (dev->tuner_type != TUNER_XC2028 && dev->tuner_type != TUNER_XC5000)
		return 0;

	if (command != XC2028_TUNER_RESET && command != XC5000_TUNER_RESET)
		return 0;

	rc = em28xx_gpio_set(dev, dev->board.tuner_gpio);

	return rc;
}