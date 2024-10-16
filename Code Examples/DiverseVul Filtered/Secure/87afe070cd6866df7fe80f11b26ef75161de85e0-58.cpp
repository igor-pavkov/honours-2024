u32 parse_boxpatch(char *arg_val, u32 opt)
{
	box_patch_filename = arg_val;
	char *sep = strchr(box_patch_filename, '=');
	if (sep) {
		sep[0] = 0;
		box_patch_trackID = atoi(box_patch_filename);
		sep[0] = '=';
		box_patch_filename = sep+1;
	}
	open_edit = GF_TRUE;
	return 0;
}