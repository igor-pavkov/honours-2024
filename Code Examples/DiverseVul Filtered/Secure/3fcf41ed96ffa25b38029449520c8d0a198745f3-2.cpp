R_API int r_bin_file_ref(RBin *bin, RBinFile *a) {
	RBinObject *o = r_bin_cur_object (bin);
	if (a && o) {
		o->referenced--;
		return true;
	}
	return false;
}