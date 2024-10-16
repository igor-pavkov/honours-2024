R_API ut64 r_bin_get_baddr(RBin *bin) {
	RBinObject *o = r_bin_cur_object (bin);
	return binobj_get_baddr (o);
}