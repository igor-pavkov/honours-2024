static int __send_write_zeroes(struct clone_info *ci)
{
	return __send_changing_extent_only(ci, get_num_write_zeroes_bios, NULL);
}