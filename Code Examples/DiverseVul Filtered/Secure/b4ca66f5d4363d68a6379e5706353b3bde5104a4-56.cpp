RPVector *r_bin_wasm_get_codes(RBinWasmObj *bin) {
	r_return_val_if_fail (bin && bin->g_sections, NULL);
	return bin->g_codes? bin->g_codes: parse_unique_subsec_vec_by_id (bin, R_BIN_WASM_SECTION_CODE);
}