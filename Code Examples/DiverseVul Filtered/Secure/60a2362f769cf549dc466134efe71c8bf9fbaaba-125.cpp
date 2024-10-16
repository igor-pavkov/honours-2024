void regulator_unregister_supply_alias(struct device *dev, const char *id)
{
	struct regulator_supply_alias *map;

	map = regulator_find_supply_alias(dev, id);
	if (map) {
		list_del(&map->list);
		kfree(map);
	}
}