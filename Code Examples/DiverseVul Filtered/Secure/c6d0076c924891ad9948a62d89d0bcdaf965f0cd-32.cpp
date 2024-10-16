void* Elf_(r_bin_elf_free)(ELFOBJ* bin) {
	int i;
	if (!bin) {
		return NULL;
	}
	free (bin->phdr);
	free (bin->shdr);
	free (bin->strtab);
	free (bin->dyn_buf);
	free (bin->shstrtab);
	free (bin->dynstr);
	//free (bin->strtab_section);
	if (bin->imports_by_ord) {
		for (i = 0; i<bin->imports_by_ord_size; i++) {
			free (bin->imports_by_ord[i]);
		}
		free (bin->imports_by_ord);
	}
	if (bin->symbols_by_ord) {
		for (i = 0; i<bin->symbols_by_ord_size; i++) {
			free (bin->symbols_by_ord[i]);
		}
		free (bin->symbols_by_ord);
	}
	r_buf_free (bin->b);
	if (bin->g_symbols != bin->phdr_symbols) {
		R_FREE (bin->phdr_symbols);
	}
	if (bin->g_imports != bin->phdr_imports) {
		R_FREE (bin->phdr_imports);
	}
	R_FREE (bin->g_sections);
	R_FREE (bin->g_symbols);
	R_FREE (bin->g_imports);
	free (bin);
	return NULL;
}