R_API RBinFile *r_bin_file_find_by_arch_bits(RBin *bin, const char *arch, int bits) {
	RListIter *iter;
	RBinFile *binfile = NULL;
	RBinXtrData *xtr_data;

	r_return_val_if_fail (bin && arch, NULL);

	r_list_foreach (bin->binfiles, iter, binfile) {
		RListIter *iter_xtr;
		if (!binfile->xtr_data) {
			continue;
		}
		// look for sub-bins in Xtr Data and Load if we need to
		r_list_foreach (binfile->xtr_data, iter_xtr, xtr_data) {
			if (xtr_metadata_match (xtr_data, arch, bits)) {
				if (!r_bin_file_object_new_from_xtr_data (bin, binfile, xtr_data->baddr,
					    xtr_data->laddr, xtr_data)) {
					return NULL;
				}
				return binfile;
			}
		}
	}
	return binfile;
}