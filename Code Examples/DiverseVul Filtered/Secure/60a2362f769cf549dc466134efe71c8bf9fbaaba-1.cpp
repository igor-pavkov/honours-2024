static int regulator_ena_gpio_request(struct regulator_dev *rdev,
				const struct regulator_config *config)
{
	struct regulator_enable_gpio *pin;
	struct gpio_desc *gpiod;
	int ret;

	gpiod = gpio_to_desc(config->ena_gpio);

	list_for_each_entry(pin, &regulator_ena_gpio_list, list) {
		if (pin->gpiod == gpiod) {
			rdev_dbg(rdev, "GPIO %d is already used\n",
				config->ena_gpio);
			goto update_ena_gpio_to_rdev;
		}
	}

	ret = gpio_request_one(config->ena_gpio,
				GPIOF_DIR_OUT | config->ena_gpio_flags,
				rdev_get_name(rdev));
	if (ret)
		return ret;

	pin = kzalloc(sizeof(struct regulator_enable_gpio), GFP_KERNEL);
	if (pin == NULL) {
		gpio_free(config->ena_gpio);
		return -ENOMEM;
	}

	pin->gpiod = gpiod;
	pin->ena_gpio_invert = config->ena_gpio_invert;
	list_add(&pin->list, &regulator_ena_gpio_list);

update_ena_gpio_to_rdev:
	pin->request_count++;
	rdev->ena_pin = pin;
	return 0;
}