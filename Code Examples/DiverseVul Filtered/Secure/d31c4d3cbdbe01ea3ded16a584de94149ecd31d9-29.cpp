R_API int r_bin_select_object(RBinFile *binfile, const char *arch, int bits,
			       const char *name) {
	RBinObject *obj = binfile ? r_bin_object_find_by_arch_bits (
					    binfile, arch, bits, name) :
				    NULL;
	return obj &&
	       r_bin_file_set_cur_binfile_obj (binfile->rbin, binfile, obj);
}