u32 parse_comp_box(char *arg_val, u32 opt)
{
	compress_top_boxes = arg_val;
	size_top_box = opt;
	return 0;
}