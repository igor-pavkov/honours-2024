R_API int r_bin_select_idx(RBin *bin, const char *name, int idx) {
	RBinFile *nbinfile = NULL, *binfile = r_bin_cur (bin);
	RBinObject *obj = NULL;
	const char *tname = !name && binfile? binfile->file: name;
	int res = false;
	if (!tname || !bin) {
		return res;
	}
	nbinfile = r_bin_file_find_by_name_n (bin, tname, idx);
	obj = nbinfile? r_list_get_n (nbinfile->objs, idx): NULL;
	return obj && nbinfile &&
	       r_bin_file_set_cur_binfile_obj (bin, nbinfile, obj);
}