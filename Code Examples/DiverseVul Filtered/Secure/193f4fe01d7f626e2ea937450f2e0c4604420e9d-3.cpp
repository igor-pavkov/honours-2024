R_API bool r_bin_file_set_cur_by_id(RBin *bin, ut32 bin_id) {
	RBinFile *bf = r_bin_file_find_by_id (bin, bin_id);
	return bf? r_bin_file_set_cur_binfile (bin, bf): false;
}