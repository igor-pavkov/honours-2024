R_API RBinObject *r_bin_file_object_get_cur(RBinFile *binfile) {
	return binfile? binfile->o: NULL;
}