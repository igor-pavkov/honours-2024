static inline void wasm_custom_local_names_free(RBinWasmCustomNameLocalNames *local) {
	if (local) {
		r_list_free (local->locals);
		R_FREE (local);
	}
}