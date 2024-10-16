static inline RBinWasmCustomNameLocalNames *parse_custom_names_local(RBuffer *b, ut64 bound) {
	RBinWasmCustomNameLocalNames *local = R_NEW0 (RBinWasmCustomNameLocalNames);
	if (!local) {
		return NULL;
	}
	if (!consume_u32_r (b, bound, &local->count)) {
		goto beach;
	}

	local->locals = r_list_newf ((RListFree)wasm_custom_name_local_free);
	if (local->locals) {
		size_t i;
		for (i = 0; i < local->count; i++) {
			RBinWasmCustomNameLocalName *local_name = parse_local_name (b, bound);
			if (!local_name || !r_list_append (local->locals, local_name)) {
				wasm_custom_name_local_free (local_name);
				goto beach;
			}
		}
		return local;
	}

beach:
	wasm_custom_local_names_free (local);
	return NULL;
}