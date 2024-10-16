struct phy_device *mdiobus_get_phy(struct mii_bus *bus, int addr)
{
	struct mdio_device *mdiodev = bus->mdio_map[addr];

	if (!mdiodev)
		return NULL;

	if (!(mdiodev->flags & MDIO_DEVICE_FLAG_PHY))
		return NULL;

	return container_of(mdiodev, struct phy_device, mdio);
}