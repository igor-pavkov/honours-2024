static ut64 binobj_get_baddr(RBinObject *o) {
	return o? o->baddr + o->baddr_shift: UT64_MAX;
}