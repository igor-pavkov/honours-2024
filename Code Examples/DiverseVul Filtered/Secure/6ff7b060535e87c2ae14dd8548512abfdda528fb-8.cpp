static int of_mdio_bus_match(struct device *dev, const void *mdio_bus_np)
{
	return dev->of_node == mdio_bus_np;
}