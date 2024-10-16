static RBinElfSymbol *Elf_(r_bin_elf_get_phdr_imports)(ELFOBJ *bin) {
	if (!bin) {
		return NULL;
	}
	if (bin->phdr_imports) {
		return bin->phdr_imports;
	}
	bin->phdr_imports = get_symbols_from_phdr (bin, R_BIN_ELF_IMPORTS);
	return bin->phdr_imports;
}