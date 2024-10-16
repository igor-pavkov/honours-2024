R_API bool r_bin_file_set_cur_binfile(RBin *bin, RBinFile *bf) {
	r_return_val_if_fail (bin && bf, false);
	return r_bin_file_set_obj (bin, bf, bf->o);
}