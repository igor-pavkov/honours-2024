static size_t consume_locals_r(RBuffer *b, ut64 bound, RBinWasmCodeEntry *out) {
	r_return_val_if_fail (out, 0);
	ut32 count = out->local_count;
	if (count <= 0) {
		return 0;
	}
	out->locals = R_NEWS0 (struct r_bin_wasm_local_entry_t, count);
	if (!out->locals) {
		return 0;
	}

	ut32 i = 0;
	for (i = 0; i < count; i++) {
		struct r_bin_wasm_local_entry_t *local = &out->locals[i];
		if (!consume_u32_r (b, bound, &local->count)) {
			return 0;
		}
		if (!consume_s7_r (b, bound, &local->type)) {
			return 0;
		}
	}
	return i;
}