struct mii_bus *devm_mdiobus_alloc_size(struct device *dev, int sizeof_priv)
{
	struct mii_bus **ptr, *bus;

	ptr = devres_alloc(_devm_mdiobus_free, sizeof(*ptr), GFP_KERNEL);
	if (!ptr)
		return NULL;

	/* use raw alloc_dr for kmalloc caller tracing */
	bus = mdiobus_alloc_size(sizeof_priv);
	if (bus) {
		*ptr = bus;
		devres_add(dev, ptr);
	} else {
		devres_free(ptr);
	}

	return bus;
}