static int em28xx_wait_until_ac97_features_equals(struct em28xx *dev,
						  int expected_feat)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(2000);
	int feat, powerdown;

	while (time_is_after_jiffies(timeout)) {
		feat = em28xx_read_ac97(dev, AC97_RESET);
		if (feat < 0)
			return feat;

		powerdown = em28xx_read_ac97(dev, AC97_POWERDOWN);
		if (powerdown < 0)
			return powerdown;

		if (feat == expected_feat && feat != powerdown)
			return 0;

		msleep(50);
	}

	dev_warn(&dev->intf->dev, "AC97 registers access is not reliable !\n");
	return -ETIMEDOUT;
}