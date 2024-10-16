int mdiobus_unregister_device(struct mdio_device *mdiodev)
{
	if (mdiodev->bus->mdio_map[mdiodev->addr] != mdiodev)
		return -EINVAL;

	mdiodev->bus->mdio_map[mdiodev->addr] = NULL;

	return 0;
}