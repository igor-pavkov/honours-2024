int regulator_bulk_register_supply_alias(struct device *dev,
					 const char *const *id,
					 struct device *alias_dev,
					 const char *const *alias_id,
					 int num_id)
{
	int i;
	int ret;

	for (i = 0; i < num_id; ++i) {
		ret = regulator_register_supply_alias(dev, id[i], alias_dev,
						      alias_id[i]);
		if (ret < 0)
			goto err;
	}

	return 0;

err:
	dev_err(dev,
		"Failed to create supply alias %s,%s -> %s,%s\n",
		id[i], dev_name(dev), alias_id[i], dev_name(alias_dev));

	while (--i >= 0)
		regulator_unregister_supply_alias(dev, id[i]);

	return ret;
}