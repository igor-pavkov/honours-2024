int mdiobus_write(struct mii_bus *bus, int addr, u32 regnum, u16 val)
{
	int err;

	BUG_ON(in_interrupt());

	mutex_lock(&bus->mdio_lock);
	err = __mdiobus_write(bus, addr, regnum, val);
	mutex_unlock(&bus->mdio_lock);

	return err;
}