ut64 Elf_(r_bin_elf_get_boffset)(ELFOBJ *bin) {
	int i;
	ut64 tmp, base = UT64_MAX;
	if (bin && bin->phdr) {
		for (i = 0; i < bin->ehdr.e_phnum; i++) {
			if (bin->phdr[i].p_type == PT_LOAD) {
				tmp =  (ut64)bin->phdr[i].p_offset & ELF_PAGE_MASK;
				tmp = tmp - (tmp % (1 << ELF_PAGE_SIZE));
				if (tmp < base) {
					base = tmp;
				}
			}
		}
	}
	return base == UT64_MAX ? 0 : base;
}