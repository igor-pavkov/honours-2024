struct phy_device *mdiobus_scan(struct mii_bus *bus, int addr)
{
	struct phy_device *phydev;
	int err;

	phydev = get_phy_device(bus, addr, false);
	if (IS_ERR(phydev))
		return phydev;

	/*
	 * For DT, see if the auto-probed phy has a correspoding child
	 * in the bus node, and set the of_node pointer in this case.
	 */
	of_mdiobus_link_mdiodev(bus, &phydev->mdio);

	err = phy_device_register(phydev);
	if (err) {
		phy_device_free(phydev);
		return ERR_PTR(-ENODEV);
	}

	return phydev;
}