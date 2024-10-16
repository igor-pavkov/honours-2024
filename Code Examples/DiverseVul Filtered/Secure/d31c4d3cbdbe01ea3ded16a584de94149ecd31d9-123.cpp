R_API int r_bin_file_deref(RBin *bin, RBinFile *a) {
	RBinObject *o = r_bin_cur_object (bin);
	int res = false;
	if (a && !o) {
		//r_list_delete_data (bin->binfiles, a);
		res = true;
	} else if (a && o->referenced - 1 < 1) {
		//r_list_delete_data (bin->binfiles, a);
		res = true;
		// not thread safe
	} else if (o) {
		o->referenced--;
	}
	// it is possible for a file not
	// to be bound to RBin and RBinFiles
	// XXX - is this an ok assumption?
	if (bin) bin->cur = NULL;
	return res;
}