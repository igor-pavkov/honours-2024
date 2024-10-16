R_API void r_bin_symbol_free(void *_sym) {
	RBinSymbol *sym = (RBinSymbol *)_sym;
	free (sym->name);
	free (sym->classname);
	free (sym);
}