static inline bool strbuf_append_type_vec(RStrBuf *sb, RBinWasmTypeVec *vec) {
	if (!r_strbuf_append (sb, "(")) {
		return false;
	}
	ut32 i;
	for (i = 0; i < vec->count; i++) {
		if (i > 0 && !r_strbuf_append (sb, ", ")) {
			return false;
		}
		const char *s = r_bin_wasm_valuetype_to_string (vec->types[i]);
		if (!s || !r_strbuf_append (sb, s)) {
			return false;
		}
	}

	if (!r_strbuf_append (sb, ")")) {
		return false;
	}
	return true;
}