static struct regulator_dev *regulator_dev_lookup(struct device *dev,
						  const char *supply,
						  int *ret)
{
	struct regulator_dev *r;
	struct device_node *node;
	struct regulator_map *map;
	const char *devname = NULL;

	regulator_supply_alias(&dev, &supply);

	/* first do a dt based lookup */
	if (dev && dev->of_node) {
		node = of_get_regulator(dev, supply);
		if (node) {
			list_for_each_entry(r, &regulator_list, list)
				if (r->dev.parent &&
					node == r->dev.of_node)
					return r;
			*ret = -EPROBE_DEFER;
			return NULL;
		} else {
			/*
			 * If we couldn't even get the node then it's
			 * not just that the device didn't register
			 * yet, there's no node and we'll never
			 * succeed.
			 */
			*ret = -ENODEV;
		}
	}

	/* if not found, try doing it non-dt way */
	if (dev)
		devname = dev_name(dev);

	list_for_each_entry(r, &regulator_list, list)
		if (strcmp(rdev_get_name(r), supply) == 0)
			return r;

	list_for_each_entry(map, &regulator_map_list, list) {
		/* If the mapping has a device set up it must match */
		if (map->dev_name &&
		    (!devname || strcmp(map->dev_name, devname)))
			continue;

		if (strcmp(map->supply, supply) == 0)
			return map->regulator;
	}


	return NULL;
}