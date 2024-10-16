static RBinWasmDataEntry *parse_data_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmDataEntry *ptr = R_NEW0 (RBinWasmDataEntry);
	if (!ptr) {
		return NULL;
	}
	ptr->sec_i = index;
	ptr->file_offset = r_buf_tell (b);
	if (!consume_u32_r (b, bound, &ptr->index)) {
		goto beach;
	}
	if (!(ptr->offset.len = consume_init_expr_r (b, bound, R_BIN_WASM_END_OF_CODE, NULL))) {
		goto beach;
	}
	if (!consume_u32_r (b, bound, &ptr->size)) {
		goto beach;
	}
	ptr->data = r_buf_tell (b);
	r_buf_seek (b, ptr->size, R_BUF_CUR);
	return ptr;

beach:
	free (ptr);
	return NULL;
}