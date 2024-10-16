static inline RPVector *parse_unique_subsec_vec_by_id(RBinWasmObj *bin, ut8 id) {
	RBinWasmSection *sec = section_by_id_unique (bin->g_sections, id);
	if (sec) {
		return parse_sub_section_vec (bin, sec);
	}
	return false;
}