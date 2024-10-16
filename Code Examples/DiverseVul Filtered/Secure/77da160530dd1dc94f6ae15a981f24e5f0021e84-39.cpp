struct hd_struct *disk_map_sector_rcu(struct gendisk *disk, sector_t sector)
{
	struct disk_part_tbl *ptbl;
	struct hd_struct *part;
	int i;

	ptbl = rcu_dereference(disk->part_tbl);

	part = rcu_dereference(ptbl->last_lookup);
	if (part && sector_in_part(part, sector))
		return part;

	for (i = 1; i < ptbl->len; i++) {
		part = rcu_dereference(ptbl->part[i]);

		if (part && sector_in_part(part, sector)) {
			rcu_assign_pointer(ptbl->last_lookup, part);
			return part;
		}
	}
	return &disk->part0;
}