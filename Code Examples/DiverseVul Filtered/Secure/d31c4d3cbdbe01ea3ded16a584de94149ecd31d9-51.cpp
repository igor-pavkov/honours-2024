R_API void r_bin_set_baddr(RBin *bin, ut64 baddr) {
	RBinObject *o = r_bin_cur_object (bin);
	binobj_set_baddr (o, baddr);
	// XXX - update all the infos?
}