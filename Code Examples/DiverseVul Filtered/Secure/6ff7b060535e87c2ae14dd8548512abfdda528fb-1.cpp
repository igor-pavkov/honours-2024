int mdiobus_read_nested(struct mii_bus *bus, int addr, u32 regnum)
{
	int retval;

	BUG_ON(in_interrupt());

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);
	retval = __mdiobus_read(bus, addr, regnum);
	mutex_unlock(&bus->mdio_lock);

	return retval;
}