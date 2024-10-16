static int mdiobus_register_gpiod(struct mdio_device *mdiodev)
{
	struct gpio_desc *gpiod = NULL;

	/* Deassert the optional reset signal */
	if (mdiodev->dev.of_node)
		gpiod = fwnode_get_named_gpiod(&mdiodev->dev.of_node->fwnode,
					       "reset-gpios", 0, GPIOD_OUT_LOW,
					       "PHY reset");
	if (PTR_ERR(gpiod) == -ENOENT ||
	    PTR_ERR(gpiod) == -ENOSYS)
		gpiod = NULL;
	else if (IS_ERR(gpiod))
		return PTR_ERR(gpiod);

	mdiodev->reset = gpiod;

	/* Assert the reset signal again */
	mdio_device_reset(mdiodev, 1);

	return 0;
}