void regulator_bulk_unregister_supply_alias(struct device *dev,
					    const char *const *id,
					    int num_id)
{
	int i;

	for (i = 0; i < num_id; ++i)
		regulator_unregister_supply_alias(dev, id[i]);
}