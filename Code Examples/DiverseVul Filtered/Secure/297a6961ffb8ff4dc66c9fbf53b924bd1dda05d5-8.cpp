static int unimac_mdio_reset(struct mii_bus *bus)
{
	struct device_node *np = bus->dev.of_node;
	struct device_node *child;
	u32 read_mask = 0;
	int addr;

	if (!np) {
		read_mask = ~bus->phy_mask;
	} else {
		for_each_available_child_of_node(np, child) {
			addr = of_mdio_parse_addr(&bus->dev, child);
			if (addr < 0)
				continue;

			read_mask |= 1 << addr;
		}
	}

	for (addr = 0; addr < PHY_MAX_ADDR; addr++) {
		if (read_mask & 1 << addr) {
			dev_dbg(&bus->dev, "Workaround for PHY @ %d\n", addr);
			mdiobus_read(bus, addr, MII_BMSR);
		}
	}

	return 0;
}