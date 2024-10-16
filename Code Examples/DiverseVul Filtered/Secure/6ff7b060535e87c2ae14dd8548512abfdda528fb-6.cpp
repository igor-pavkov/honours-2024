int mdiobus_read(struct mii_bus *bus, int addr, u32 regnum)
{
	int retval;

	BUG_ON(in_interrupt());

	mutex_lock(&bus->mdio_lock);
	retval = __mdiobus_read(bus, addr, regnum);
	mutex_unlock(&bus->mdio_lock);

	return retval;
}