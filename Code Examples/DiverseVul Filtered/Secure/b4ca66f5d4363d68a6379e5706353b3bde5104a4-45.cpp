static ut32 r_bin_wasm_get_start(RBinWasmObj *bin) {
	if (bin->g_start == UT32_MAX) {
		RBinWasmSection *sec = section_by_id_unique (bin->g_sections, R_BIN_WASM_SECTION_START);
		if (sec) {
			RBuffer *b = bin->buf;
			r_buf_seek (b, sec->payload_data, R_BUF_SET);
			ut64 bound = r_buf_tell (b) + sec->payload_len - 1;
			if (!consume_u32_r (b, bound, &bin->g_start)) {
				bin->g_start = UT32_MAX;
			}
		}
	}
	return bin->g_start;
}