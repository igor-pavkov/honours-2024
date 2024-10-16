static ut64 binobj_a2b(RBinObject *o, ut64 addr) {
	return addr + (o? o->baddr_shift: 0);
}