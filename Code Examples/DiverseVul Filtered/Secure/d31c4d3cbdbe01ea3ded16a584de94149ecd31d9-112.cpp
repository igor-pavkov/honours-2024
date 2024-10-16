R_API int r_bin_select(RBin *bin, const char *arch, int bits,
			const char *name) {
	RBinFile *cur = r_bin_cur (bin), *binfile = NULL;
	RBinObject *obj = NULL;
	name = !name && cur? cur->file: name;
	binfile = r_bin_file_find_by_arch_bits (bin, arch, bits, name);
	if (binfile && name) {
		obj = r_bin_object_find_by_arch_bits (binfile, arch, bits, name);
	}
	return binfile && r_bin_file_set_cur_binfile_obj (bin, binfile, obj);
}