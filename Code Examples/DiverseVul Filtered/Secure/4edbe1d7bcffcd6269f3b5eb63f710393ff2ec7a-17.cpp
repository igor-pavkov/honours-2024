static void retrieve_deps(struct dm_table *table,
			  struct dm_ioctl *param, size_t param_size)
{
	unsigned int count = 0;
	struct list_head *tmp;
	size_t len, needed;
	struct dm_dev_internal *dd;
	struct dm_target_deps *deps;

	deps = get_result_buffer(param, param_size, &len);

	/*
	 * Count the devices.
	 */
	list_for_each (tmp, dm_table_get_devices(table))
		count++;

	/*
	 * Check we have enough space.
	 */
	needed = struct_size(deps, dev, count);
	if (len < needed) {
		param->flags |= DM_BUFFER_FULL_FLAG;
		return;
	}

	/*
	 * Fill in the devices.
	 */
	deps->count = count;
	count = 0;
	list_for_each_entry (dd, dm_table_get_devices(table), list)
		deps->dev[count++] = huge_encode_dev(dd->dm_dev->bdev->bd_dev);

	param->data_size = param->data_start + needed;
}