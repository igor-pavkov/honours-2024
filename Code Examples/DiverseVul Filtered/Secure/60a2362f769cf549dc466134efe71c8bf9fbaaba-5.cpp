static ssize_t supply_map_read_file(struct file *file, char __user *user_buf,
				    size_t count, loff_t *ppos)
{
	char *buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	ssize_t len, ret = 0;
	struct regulator_map *map;

	if (!buf)
		return -ENOMEM;

	list_for_each_entry(map, &regulator_map_list, list) {
		len = snprintf(buf + ret, PAGE_SIZE - ret,
			       "%s -> %s.%s\n",
			       rdev_get_name(map->regulator), map->dev_name,
			       map->supply);
		if (len >= 0)
			ret += len;
		if (ret > PAGE_SIZE) {
			ret = PAGE_SIZE;
			break;
		}
	}

	ret = simple_read_from_buffer(user_buf, count, ppos, buf, ret);

	kfree(buf);

	return ret;
}