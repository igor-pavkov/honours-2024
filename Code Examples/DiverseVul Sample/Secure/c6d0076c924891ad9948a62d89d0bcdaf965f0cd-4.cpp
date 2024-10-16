char *Elf_(r_bin_elf_intrp)(ELFOBJ *bin) {
	int i;
	if (!bin || !bin->phdr) {
		return NULL;
	}
	for (i = 0; i < bin->ehdr.e_phnum; i++) {
		if (bin->phdr[i].p_type == PT_INTERP) {
			char *str = NULL;
			ut64 addr = bin->phdr[i].p_offset;
			int sz = bin->phdr[i].p_memsz;
			sdb_num_set (bin->kv, "elf_header.intrp_addr", addr, 0);
			sdb_num_set (bin->kv, "elf_header.intrp_size", sz, 0);
			if (sz < 1) {
				return NULL;
			}
			str = malloc (sz + 1);
			if (!str) {
				return NULL;
			}
			if (r_buf_read_at (bin->b, addr, (ut8*)str, sz) < 1) {
				bprintf ("Warning: read (main)\n");
				return 0;
			}
			str[sz] = 0;
			sdb_set (bin->kv, "elf_header.intrp", str, 0);
			return str;
		}
	}
	return NULL;
}