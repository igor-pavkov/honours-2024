static int r_bin_file_object_add(RBinFile *binfile, RBinObject *o) {
	if (!o) {
		return false;
	}
	r_list_append (binfile->objs, o);
	r_bin_file_set_cur_binfile_obj (binfile->rbin, binfile, o);
	return true;
}