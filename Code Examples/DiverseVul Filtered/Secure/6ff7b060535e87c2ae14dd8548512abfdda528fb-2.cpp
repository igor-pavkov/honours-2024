static int mdio_bus_match(struct device *dev, struct device_driver *drv)
{
	struct mdio_device *mdio = to_mdio_device(dev);

	if (of_driver_match_device(dev, drv))
		return 1;

	if (mdio->bus_match)
		return mdio->bus_match(dev, drv);

	return 0;
}