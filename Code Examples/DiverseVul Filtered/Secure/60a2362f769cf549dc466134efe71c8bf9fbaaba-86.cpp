static struct regulator_supply_alias *regulator_find_supply_alias(
		struct device *dev, const char *supply)
{
	struct regulator_supply_alias *map;

	list_for_each_entry(map, &regulator_supply_alias_list, list)
		if (map->src_dev == dev && strcmp(map->src_supply, supply) == 0)
			return map;

	return NULL;
}