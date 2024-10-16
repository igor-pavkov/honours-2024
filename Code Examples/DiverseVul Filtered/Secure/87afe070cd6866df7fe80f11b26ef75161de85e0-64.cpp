Bool mp4box_parse_single_arg(int argc, char **argv, char *arg, u32 *arg_index)
{
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_gen_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_split_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_dash_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_imp_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_senc_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_crypt_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_hint_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_extr_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_dump_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_meta_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_swf_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_liveenc_args)) return GF_TRUE;
	if (mp4box_parse_single_arg_class(argc, argv, arg, arg_index, m4b_usage_args)) return GF_TRUE;

	return GF_FALSE;
}