static struct dm_target *dm_dax_get_live_target(struct mapped_device *md,
		sector_t sector, int *srcu_idx)
{
	struct dm_table *map;
	struct dm_target *ti;

	map = dm_get_live_table(md, srcu_idx);
	if (!map)
		return NULL;

	ti = dm_table_find_target(map, sector);
	if (!dm_target_is_valid(ti))
		return NULL;

	return ti;
}