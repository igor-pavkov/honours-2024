R_API int r_bin_file_set_cur_by_name(RBin *bin, const char *name) {
	RBinFile *bf = r_bin_file_find_by_name (bin, name);
	return r_bin_file_set_cur_binfile (bin, bf);
}