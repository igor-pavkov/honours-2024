ut32 r_bin_wasm_get_entrypoint(RBinWasmObj *bin) {
	r_return_val_if_fail (bin && bin->g_sections, 0);

	if (bin->entrypoint) {
		return bin->entrypoint;
	}
	ut32 start = r_bin_wasm_get_start (bin);
	RPVector *code = r_bin_wasm_get_codes (bin);
	// FIX: entrypoint can be also an import
	if (code && start != UT32_MAX) {
		RBinWasmCodeEntry *func = r_pvector_at (code, start);
		return func? func->code: 0;
	}
	return 0;
}