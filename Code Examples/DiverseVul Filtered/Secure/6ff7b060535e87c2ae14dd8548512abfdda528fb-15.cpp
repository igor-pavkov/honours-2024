int __mdiobus_write(struct mii_bus *bus, int addr, u32 regnum, u16 val)
{
	int err;

	WARN_ON_ONCE(!mutex_is_locked(&bus->mdio_lock));

	err = bus->write(bus, addr, regnum, val);

	trace_mdio_access(bus, 0, addr, regnum, val, err);

	return err;
}