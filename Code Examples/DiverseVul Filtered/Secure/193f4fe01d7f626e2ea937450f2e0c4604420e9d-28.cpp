static ut64 binobj_a2b(RBinObject *bo, ut64 addr) {
	return addr + (bo ? bo->baddr_shift : 0);
}