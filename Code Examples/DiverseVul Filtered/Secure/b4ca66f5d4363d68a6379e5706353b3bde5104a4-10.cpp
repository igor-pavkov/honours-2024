static void wasm_custom_name_local_free(RBinWasmCustomNameLocalName *name) {
	if (name) {
		r_id_storage_free (name->names);
		R_FREE (name);
	}
}