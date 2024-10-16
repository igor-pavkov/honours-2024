static RBinWasmSection *section_by_id_unique(RList *sections, ut8 id) {
	RList *l = r_bin_wasm_get_sections_by_id (sections, id);
	RBinWasmSection *sec = NULL;
	if (l) {
		int len = r_list_length (l);
		if (len) {
			sec = r_list_first (l);
			if (sec && len > 1) {
				eprintf ("[wasm] Using first %s section of %d\n", sec->name, len);
			}
		}
	}
	r_list_free (l);
	return sec;
}