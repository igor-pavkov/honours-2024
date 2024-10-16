RPVector *r_bin_wasm_get_elements(RBinWasmObj *bin) {
	r_return_val_if_fail (bin && bin->g_sections, NULL);
	return bin->g_elements? bin->g_elements: parse_unique_subsec_vec_by_id (bin, R_BIN_WASM_SECTION_ELEMENT);
}