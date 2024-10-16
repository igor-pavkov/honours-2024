static void binobj_set_baddr(RBinObject *o, ut64 baddr) {
	if (!o || baddr == UT64_MAX) {
		return;
	}
	o->baddr_shift = baddr - o->baddr;
}