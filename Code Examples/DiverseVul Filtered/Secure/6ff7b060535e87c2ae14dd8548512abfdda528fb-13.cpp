static void of_mdiobus_link_mdiodev(struct mii_bus *bus,
				    struct mdio_device *mdiodev)
{
	struct device *dev = &mdiodev->dev;
	struct device_node *child;

	if (dev->of_node || !bus->dev.of_node)
		return;

	for_each_available_child_of_node(bus->dev.of_node, child) {
		int addr;

		addr = of_mdio_parse_addr(dev, child);
		if (addr < 0)
			continue;

		if (addr == mdiodev->addr) {
			dev->of_node = child;
			dev->fwnode = of_fwnode_handle(child);
			return;
		}
	}
}