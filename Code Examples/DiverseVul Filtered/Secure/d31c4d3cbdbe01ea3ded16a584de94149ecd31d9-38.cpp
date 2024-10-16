R_API ut64 r_bin_a2b(RBin *bin, ut64 addr) {
	RBinObject *o = r_bin_cur_object (bin);
	return o? o->baddr_shift + addr: addr;
}