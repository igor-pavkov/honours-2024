R_API int r_bin_object_delete(RBin *bin, ut32 binfile_id, ut32 binobj_id) {
	RBinFile *binfile = NULL; //, *cbinfile = r_bin_cur (bin);
	RBinObject *obj = NULL;
	int res = false;

#if 0
	if (binfile_id == UT32_MAX && binobj_id == UT32_MAX) {
		return false;
	}
#endif
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

	// lazy way out, always leaving at least 1 bin object loaded
	if (binfile && (r_list_length (binfile->objs) > 1)) {
		binfile->o = NULL;
		r_list_delete_data (binfile->objs, obj);
		obj = (RBinObject *)r_list_get_n (binfile->objs, 0);
		res = obj && binfile &&
		      r_bin_file_set_cur_binfile_obj (bin, binfile, obj);
	}
	return res;
}