R_API RBinObject *r_bin_cur_object(RBin *bin) {
	RBinFile *binfile = r_bin_cur (bin);
	return binfile? binfile->o: NULL;
}