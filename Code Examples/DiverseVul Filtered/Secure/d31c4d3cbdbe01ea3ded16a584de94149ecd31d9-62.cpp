R_API RBinSymbol *r_bin_get_symbol_at_vaddr(RBin *bin, ut64 addr) {
	//use skiplist here
	RList *symbols = r_bin_get_symbols (bin);
	RListIter *iter;
	RBinSymbol *symbol;
	r_list_foreach (symbols, iter, symbol) {
		if (symbol->vaddr == addr) {
			return symbol;
		}
	}
	return NULL;
}