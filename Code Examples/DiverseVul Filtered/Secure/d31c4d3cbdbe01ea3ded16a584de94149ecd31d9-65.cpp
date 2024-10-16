R_API int r_bin_file_set_cur_binfile(RBin *bin, RBinFile *bf) {
	RBinObject *obj = bf? bf->o: NULL;
	return obj? r_bin_file_set_cur_binfile_obj (bin, bf, obj): false;
}