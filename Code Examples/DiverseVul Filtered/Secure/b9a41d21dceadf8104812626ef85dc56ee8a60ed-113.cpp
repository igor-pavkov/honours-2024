static int __send_discard(struct clone_info *ci)
{
	return __send_changing_extent_only(ci, get_num_discard_bios,
					   is_split_required_for_discard);
}