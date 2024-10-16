const char *r_bin_wasm_get_function_name(RBinWasmObj *bin, ut32 idx) {
	if (!(bin && bin->g_names)) {
		return NULL;
	};

	RListIter *iter;
	RBinWasmCustomNameEntry *nam;
	r_list_foreach (bin->g_names, iter, nam) {
		if (nam->type == R_BIN_WASM_NAMETYPE_Function) {
			const char *n = r_id_storage_get (nam->func->names, idx);
			if (n) {
				return n;
			}
		}
	}

	return NULL;
}