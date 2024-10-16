static bool append_rets(RStrBuf *sb, RBinWasmTypeVec *rets) {
	bool ret = true;
	if (!rets->count) {
		ret &= r_strbuf_append (sb, "nil");
	} else if (rets->count == 1) {
		ret &= r_strbuf_append (sb, r_bin_wasm_valuetype_to_string (rets->types[0]));
	} else {
		ret &= strbuf_append_type_vec (sb, rets);
	}
	return ret;
}