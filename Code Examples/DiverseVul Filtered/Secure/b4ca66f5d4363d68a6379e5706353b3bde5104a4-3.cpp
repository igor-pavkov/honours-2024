RBinWasmObj *r_bin_wasm_init(RBinFile *bf, RBuffer *buf) {
	RBinWasmObj *bin = R_NEW0 (RBinWasmObj);
	if (bin) {
		bin->g_start = UT32_MAX;
		bin->buf = r_buf_ref (buf);
		bin->size = (ut32)r_buf_size (bf->buf);
		bin->g_sections = r_bin_wasm_get_sections (bin);
		// TODO: recursive invocation more natural with streamed parsing
		// but dependency problems when sections are disordered (against spec)

		bin->g_types = r_bin_wasm_get_types (bin);
		bin->g_imports = r_bin_wasm_get_imports (bin);
		bin->g_funcs = r_bin_wasm_get_functions (bin);
		bin->g_tables = r_bin_wasm_get_tables (bin);
		bin->g_memories = r_bin_wasm_get_memories (bin);
		bin->g_globals = r_bin_wasm_get_globals (bin);
		bin->g_exports = r_bin_wasm_get_exports (bin);
		bin->g_codes = r_bin_wasm_get_codes (bin);
		bin->g_datas = r_bin_wasm_get_datas (bin);

		bin->g_names = r_bin_wasm_get_custom_names (bin);

		// entrypoint from Start section
		bin->entrypoint = r_bin_wasm_get_entrypoint (bin);
	}
	return bin;
}