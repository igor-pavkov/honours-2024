R_API bool r_bin_file_deref(RBin *bin, RBinFile *a) {
	r_return_val_if_fail (bin && a, false);
	if (!r_bin_cur_object (bin)) {
		return false;
	}
	bin->cur = NULL;
	return true;
}