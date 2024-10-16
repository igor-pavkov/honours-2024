static RBinAddr *newEntry(ut64 haddr, ut64 vaddr, int type) {
	RBinAddr *ptr = R_NEW0 (RBinAddr);
	if (!ptr) {
		return NULL;
	}
	ptr->paddr = haddr;
	ptr->vaddr = vaddr;
	ptr->hpaddr = haddr;
	ptr->bits = 64;
	ptr->type = type;
	return ptr;
}