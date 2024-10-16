static sector_t max_io_len_target_boundary(sector_t sector, struct dm_target *ti)
{
	sector_t target_offset = dm_target_offset(ti, sector);

	return ti->len - target_offset;
}