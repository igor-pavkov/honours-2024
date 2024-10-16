static unsigned get_num_write_zeroes_bios(struct dm_target *ti)
{
	return ti->num_write_zeroes_bios;
}