int __mdiobus_read(struct mii_bus *bus, int addr, u32 regnum)
{
	int retval;

	WARN_ON_ONCE(!mutex_is_locked(&bus->mdio_lock));

	retval = bus->read(bus, addr, regnum);

	trace_mdio_access(bus, 1, addr, regnum, retval, retval);

	return retval;
}