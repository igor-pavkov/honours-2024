R_API RBinObject *r_bin_object_get_cur(RBin *bin) {
	return bin
		? r_bin_file_object_get_cur (r_bin_cur (bin))
		: NULL;
}