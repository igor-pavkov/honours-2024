R_API ut64 r_bin_file_get_baddr(RBinFile *binfile) {
	return binfile? r_bin_object_get_baddr (binfile->o): UT64_MAX;
}