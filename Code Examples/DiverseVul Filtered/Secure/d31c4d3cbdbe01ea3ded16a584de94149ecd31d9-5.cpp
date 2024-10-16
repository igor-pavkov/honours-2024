R_API int r_bin_dump_strings(RBinFile *a, int min) {
	get_strings (a, min, 1);
	return 0;
}