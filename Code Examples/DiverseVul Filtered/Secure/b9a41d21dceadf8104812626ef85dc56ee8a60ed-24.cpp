static int __send_changing_extent_only(struct clone_info *ci,
				       get_num_bios_fn get_num_bios,
				       is_split_required_fn is_split_required)
{
	struct dm_target *ti;
	unsigned len;
	unsigned num_bios;

	do {
		ti = dm_table_find_target(ci->map, ci->sector);
		if (!dm_target_is_valid(ti))
			return -EIO;

		/*
		 * Even though the device advertised support for this type of
		 * request, that does not mean every target supports it, and
		 * reconfiguration might also have changed that since the
		 * check was performed.
		 */
		num_bios = get_num_bios ? get_num_bios(ti) : 0;
		if (!num_bios)
			return -EOPNOTSUPP;

		if (is_split_required && !is_split_required(ti))
			len = min((sector_t)ci->sector_count, max_io_len_target_boundary(ci->sector, ti));
		else
			len = min((sector_t)ci->sector_count, max_io_len(ci->sector, ti));

		__send_duplicate_bios(ci, ti, num_bios, &len);

		ci->sector += len;
	} while (ci->sector_count -= len);

	return 0;
}