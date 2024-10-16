R_API ut64 r_binfile_get_baddr(RBinFile *binfile) {
	return binfile? binobj_get_baddr (binfile->o): UT64_MAX;
}