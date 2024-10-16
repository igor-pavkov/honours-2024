u32 parse_cryp(char *arg_val, u32 opt)
{
	open_edit = GF_TRUE;
	if (!opt) {
		crypt = 1;
		drm_file = arg_val;
		open_edit = GF_TRUE;
		return 0;
	}
	crypt = 2;
	if (arg_val && get_file_type_by_ext(arg_val) != 1) {
		drm_file = arg_val;
		return 0;
	}
	return 3;
}