RPVector *r_bin_wasm_get_imports(RBinWasmObj *bin) {
	r_return_val_if_fail (bin && bin->g_sections, NULL);
	return bin->g_imports? bin->g_imports: parse_unique_subsec_vec_by_id (bin, R_BIN_WASM_SECTION_IMPORT);
}