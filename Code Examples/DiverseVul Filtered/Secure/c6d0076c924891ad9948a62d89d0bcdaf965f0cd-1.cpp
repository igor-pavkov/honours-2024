int Elf_(r_bin_elf_get_stripped)(ELFOBJ *bin) {
	int i;
	if (!bin->shdr) {
		return false;
	}
	for (i = 0; i < bin->ehdr.e_shnum; i++) {
		if (bin->shdr[i].sh_type == SHT_SYMTAB) {
			return false;
		}
	}
	return true;
}