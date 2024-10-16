void devm_mdiobus_free(struct device *dev, struct mii_bus *bus)
{
	int rc;

	rc = devres_release(dev, _devm_mdiobus_free,
			    devm_mdiobus_match, bus);
	WARN_ON(rc);
}