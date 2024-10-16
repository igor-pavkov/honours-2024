R_API int r_bin_file_deref_by_bind(RBinBind *binb) {
	RBin *bin = binb? binb->bin: NULL;
	RBinFile *a = r_bin_cur (bin);
	return r_bin_file_deref (bin, a);
}