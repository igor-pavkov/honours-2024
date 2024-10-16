void mdiobus_unregister(struct mii_bus *bus)
{
	struct mdio_device *mdiodev;
	int i;

	BUG_ON(bus->state != MDIOBUS_REGISTERED);
	bus->state = MDIOBUS_UNREGISTERED;

	for (i = 0; i < PHY_MAX_ADDR; i++) {
		mdiodev = bus->mdio_map[i];
		if (!mdiodev)
			continue;

		if (mdiodev->reset)
			gpiod_put(mdiodev->reset);

		mdiodev->device_remove(mdiodev);
		mdiodev->device_free(mdiodev);
	}

	/* Put PHYs in RESET to save power */
	if (bus->reset_gpiod)
		gpiod_set_value_cansleep(bus->reset_gpiod, 1);

	device_del(&bus->dev);
}