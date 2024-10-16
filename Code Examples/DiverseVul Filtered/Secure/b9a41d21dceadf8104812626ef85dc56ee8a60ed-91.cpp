static bool is_split_required_for_discard(struct dm_target *ti)
{
	return ti->split_discard_bios;
}