static int __send_write_same(struct clone_info *ci)
{
	return __send_changing_extent_only(ci, get_num_write_same_bios, NULL);
}