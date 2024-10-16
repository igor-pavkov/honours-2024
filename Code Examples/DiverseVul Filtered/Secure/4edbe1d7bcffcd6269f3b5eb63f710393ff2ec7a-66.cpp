static void __dev_status(struct mapped_device *md, struct dm_ioctl *param)
{
	struct gendisk *disk = dm_disk(md);
	struct dm_table *table;
	int srcu_idx;

	param->flags &= ~(DM_SUSPEND_FLAG | DM_READONLY_FLAG |
			  DM_ACTIVE_PRESENT_FLAG | DM_INTERNAL_SUSPEND_FLAG);

	if (dm_suspended_md(md))
		param->flags |= DM_SUSPEND_FLAG;

	if (dm_suspended_internally_md(md))
		param->flags |= DM_INTERNAL_SUSPEND_FLAG;

	if (dm_test_deferred_remove_flag(md))
		param->flags |= DM_DEFERRED_REMOVE;

	param->dev = huge_encode_dev(disk_devt(disk));

	/*
	 * Yes, this will be out of date by the time it gets back
	 * to userland, but it is still very useful for
	 * debugging.
	 */
	param->open_count = dm_open_count(md);

	param->event_nr = dm_get_event_nr(md);
	param->target_count = 0;

	table = dm_get_live_table(md, &srcu_idx);
	if (table) {
		if (!(param->flags & DM_QUERY_INACTIVE_TABLE_FLAG)) {
			if (get_disk_ro(disk))
				param->flags |= DM_READONLY_FLAG;
			param->target_count = dm_table_get_num_targets(table);
		}

		param->flags |= DM_ACTIVE_PRESENT_FLAG;
	}
	dm_put_live_table(md, srcu_idx);

	if (param->flags & DM_QUERY_INACTIVE_TABLE_FLAG) {
		int srcu_idx;
		table = dm_get_inactive_table(md, &srcu_idx);
		if (table) {
			if (!(dm_table_get_mode(table) & FMODE_WRITE))
				param->flags |= DM_READONLY_FLAG;
			param->target_count = dm_table_get_num_targets(table);
		}
		dm_put_live_table(md, srcu_idx);
	}
}