ut64 Elf_(r_bin_elf_get_section_addr)(ELFOBJ *bin, const char *section_name) {
	RBinElfSection *section = get_section_by_name (bin, section_name);
	return section? section->rva: UT64_MAX;
}