static const char *r_bin_wasm_type_entry_to_string(RBinWasmTypeEntry *type) {
	r_return_val_if_fail (type, NULL);
	if (type->to_str) {
		return type->to_str;
	}

	RStrBuf *sb = r_strbuf_new ("");
	if (!sb) {
		return NULL;
	}
	r_strbuf_reserve (sb, (type->args->count + type->rets->count) * 8);

	bool appended = strbuf_append_type_vec (sb, type->args);
	appended &= r_strbuf_append (sb, " -> ");
	appended &= append_rets (sb, type->rets);
	if (appended) {
		type->to_str = r_strbuf_drain (sb);
	} else {
		r_strbuf_free (sb);
	}
	return type->to_str;
}