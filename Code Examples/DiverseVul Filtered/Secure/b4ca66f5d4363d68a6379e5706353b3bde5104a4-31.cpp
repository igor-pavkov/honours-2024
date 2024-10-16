static inline RBinWasmCustomNameLocalName *parse_local_name(RBuffer *b, ut64 bound) {
	RBinWasmCustomNameLocalName *local_name = R_NEW0 (RBinWasmCustomNameLocalName);
	if (local_name) {
		if (!consume_u32_r (b, bound, &local_name->index)) {
			goto beach;
		}

		local_name->names = r_id_storage_new (0, UT32_MAX);
		if (!local_name->names) {
			goto beach;
		}

		if (!parse_namemap (b, bound, local_name->names, &local_name->names_count)) {
			goto beach;
		}

		return local_name;
	}
beach:
	wasm_custom_name_local_free (local_name);
	return NULL;
}