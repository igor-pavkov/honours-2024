char* Elf_(r_bin_elf_get_elf_class)(ELFOBJ *bin) {
	switch (bin->ehdr.e_ident[EI_CLASS]) {
	case ELFCLASSNONE: return strdup ("none");
	case ELFCLASS32:   return strdup ("ELF32");
	case ELFCLASS64:   return strdup ("ELF64");
	default:           return r_str_newf ("<unknown: %x>", bin->ehdr.e_ident[EI_CLASS]);
	}
}