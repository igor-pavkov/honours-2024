static unsigned get_num_write_same_bios(struct dm_target *ti)
{
	return ti->num_write_same_bios;
}