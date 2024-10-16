static void _devm_mdiobus_free(struct device *dev, void *res)
{
	mdiobus_free(*(struct mii_bus **)res);
}