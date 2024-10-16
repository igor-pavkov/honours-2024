int regulator_register_supply_alias(struct device *dev, const char *id,
				    struct device *alias_dev,
				    const char *alias_id)
{
	struct regulator_supply_alias *map;

	map = regulator_find_supply_alias(dev, id);
	if (map)
		return -EEXIST;

	map = kzalloc(sizeof(struct regulator_supply_alias), GFP_KERNEL);
	if (!map)
		return -ENOMEM;

	map->src_dev = dev;
	map->src_supply = id;
	map->alias_dev = alias_dev;
	map->alias_supply = alias_id;

	list_add(&map->list, &regulator_supply_alias_list);

	pr_info("Adding alias for supply %s,%s -> %s,%s\n",
		id, dev_name(dev), alias_id, dev_name(alias_dev));

	return 0;
}