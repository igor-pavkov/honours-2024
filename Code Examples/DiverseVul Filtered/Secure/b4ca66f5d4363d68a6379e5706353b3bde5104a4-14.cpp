static RBinWasmGlobalEntry *parse_global_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmGlobalEntry *ptr = R_NEW0 (RBinWasmGlobalEntry);
	if (ptr) {
		ptr->sec_i = index;
		ptr->file_offset = r_buf_tell (b);
		if (!consume_u7_r (b, bound, (ut8 *)&ptr->content_type)) {
			goto beach;
		}
		if (!consume_u1_r (b, bound, &ptr->mutability)) {
			goto beach;
		}
		if (!consume_init_expr_r (b, bound, R_BIN_WASM_END_OF_CODE, NULL)) {
			goto beach;
		}
	}
	return ptr;

beach:
	free (ptr);
	return NULL;
}