static RList *r_bin_wasm_get_sections_by_id(RList *sections, ut8 id) {
	RList *ret = r_list_newf (NULL);
	if (ret) {
		RBinWasmSection *sec;
		RListIter *iter;
		r_list_foreach (sections, iter, sec) {
			if (sec->id == id) {
				r_list_append (ret, sec);
			}
		}
	}
	return ret;
}