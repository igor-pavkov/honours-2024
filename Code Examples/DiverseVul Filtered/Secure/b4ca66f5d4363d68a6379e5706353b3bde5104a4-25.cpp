static void wasm_sec_free(RBinWasmSection *sec) {
	if (sec) {
		free (sec->name);
		free (sec);
	}
}