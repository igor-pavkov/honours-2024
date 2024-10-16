static int mdiobus_create_device(struct mii_bus *bus,
				 struct mdio_board_info *bi)
{
	struct mdio_device *mdiodev;
	int ret = 0;

	mdiodev = mdio_device_create(bus, bi->mdio_addr);
	if (IS_ERR(mdiodev))
		return -ENODEV;

	strncpy(mdiodev->modalias, bi->modalias,
		sizeof(mdiodev->modalias));
	mdiodev->bus_match = mdio_device_bus_match;
	mdiodev->dev.platform_data = (void *)bi->platform_data;

	ret = mdio_device_register(mdiodev);
	if (ret)
		mdio_device_free(mdiodev);

	return ret;
}