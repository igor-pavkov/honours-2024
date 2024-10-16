static int devm_mdiobus_match(struct device *dev, void *res, void *data)
{
	struct mii_bus **r = res;

	if (WARN_ON(!r || !*r))
		return 0;

	return *r == data;
}