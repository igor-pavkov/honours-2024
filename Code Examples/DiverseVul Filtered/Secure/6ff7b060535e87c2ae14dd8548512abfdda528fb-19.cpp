int mdiobus_register_device(struct mdio_device *mdiodev)
{
	int err;

	if (mdiodev->bus->mdio_map[mdiodev->addr])
		return -EBUSY;

	if (mdiodev->flags & MDIO_DEVICE_FLAG_PHY) {
		err = mdiobus_register_gpiod(mdiodev);
		if (err)
			return err;
	}

	mdiodev->bus->mdio_map[mdiodev->addr] = mdiodev;

	return 0;
}