bool mdiobus_is_registered_device(struct mii_bus *bus, int addr)
{
	return bus->mdio_map[addr];
}