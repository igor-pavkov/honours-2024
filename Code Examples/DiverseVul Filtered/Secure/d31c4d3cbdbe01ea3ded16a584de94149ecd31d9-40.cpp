R_API int r_bin_select_by_ids(RBin *bin, ut32 binfile_id, ut32 binobj_id) {
	RBinFile *binfile = NULL;
	RBinObject *obj = NULL;

	if (binfile_id == UT32_MAX && binobj_id == UT32_MAX) {
		return false;
	}
	if (binfile_id == -1) {
		binfile = r_bin_file_find_by_object_id (bin, binobj_id);
		obj = binfile? r_bin_file_object_find_by_id (binfile, binobj_id): NULL;
	} else if (binobj_id == -1) {
		binfile = r_bin_file_find_by_id (bin, binfile_id);
		obj = binfile? binfile->o: NULL;
	} else {
		binfile = r_bin_file_find_by_id (bin, binfile_id);
		obj = binfile? r_bin_file_object_find_by_id (binfile, binobj_id): NULL;
	}
	if (!binfile || !obj) {
		return false;
	}
	return obj && binfile && r_bin_file_set_cur_binfile_obj (bin, binfile, obj);
}