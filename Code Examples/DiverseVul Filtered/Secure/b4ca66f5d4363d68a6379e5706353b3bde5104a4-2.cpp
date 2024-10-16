static RList *r_bin_wasm_get_custom_name_entries(RBinWasmObj *bin, RBinWasmSection *sec) {
	RList *ret = r_list_newf ((RListFree)wasm_custom_name_free);

	RBuffer *buf = bin->buf;

	r_buf_seek (buf, sec->payload_data, R_BUF_SET);
	ut64 bound = sec->payload_data + sec->payload_len - 1;

	if (bound > r_buf_size (buf)) {
		goto beach;
	}

	while (r_buf_tell (buf) < bound) {
		RBinWasmCustomNameEntry *nam = parse_custom_name_entry (bin, bound);

		if (!nam) {
			break; // allow partial parsing of section
		}

		if (!r_list_append (ret, nam)) {
			goto beach;
		}
	}

	return ret;
beach:
	r_list_free (ret);
	return NULL;
}