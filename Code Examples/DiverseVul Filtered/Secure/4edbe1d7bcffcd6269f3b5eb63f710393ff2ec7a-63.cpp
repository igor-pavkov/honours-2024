static int table_status(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	struct mapped_device *md;
	struct dm_table *table;
	int srcu_idx;

	md = find_device(param);
	if (!md)
		return -ENXIO;

	__dev_status(md, param);

	table = dm_get_live_or_inactive_table(md, param, &srcu_idx);
	if (table)
		retrieve_status(table, param, param_size);
	dm_put_live_table(md, srcu_idx);

	dm_put(md);

	return 0;
}