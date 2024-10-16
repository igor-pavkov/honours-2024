R_API ut64 r_bin_get_laddr(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->loadaddr: UT64_MAX;
}