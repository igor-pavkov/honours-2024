R_API ut64 r_bin_file_get_baddr(RBinFile *bf) {
	if (bf && bf->o) {
		return bf->o->baddr;
	}
	return UT64_MAX;
}