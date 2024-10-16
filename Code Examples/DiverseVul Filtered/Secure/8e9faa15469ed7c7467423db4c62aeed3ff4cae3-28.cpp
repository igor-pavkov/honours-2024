static int cp2112_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	int ret;

	ret = cp2112_gpio_get_all(chip);
	if (ret < 0)
		return ret;

	return (ret >> offset) & 1;
}