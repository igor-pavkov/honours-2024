static RBinWasmElementEntry *parse_element_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmElementEntry *elem = R_NEW0 (RBinWasmElementEntry);
	if (elem) {
		elem->sec_i = index;
		elem->file_offset = r_buf_tell (b);
		if (!consume_u32_r (b, bound, &elem->index)) {
			goto beach;
		}
		if (!consume_init_expr_r (b, bound, R_BIN_WASM_END_OF_CODE, NULL)) {
			goto beach;
		}
		if (!consume_u32_r (b, bound, &elem->num_elem)) {
			goto beach;
		}
		ut32 j = 0;
		while (r_buf_tell (b) <= bound && j < elem->num_elem) {
			// TODO: allocate space and fill entry
			if (!consume_u32_r (b, bound, NULL)) {
				goto beach;
			}
		}
	}
	return elem;

beach:
	free (elem);
	return NULL;
}