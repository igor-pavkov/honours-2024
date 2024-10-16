static struct dm_table *dm_get_live_or_inactive_table(struct mapped_device *md,
						      struct dm_ioctl *param,
						      int *srcu_idx)
{
	return (param->flags & DM_QUERY_INACTIVE_TABLE_FLAG) ?
		dm_get_inactive_table(md, srcu_idx) : dm_get_live_table(md, srcu_idx);
}