static RBinWasmCodeEntry *parse_code_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmCodeEntry *ptr = R_NEW0 (RBinWasmCodeEntry);
	if (!ptr) {
		return NULL;
	}
	ptr->sec_i = index;
	ptr->file_offset = r_buf_tell (b);
	if (!consume_u32_r (b, bound, &ptr->body_size)) {
		goto beach;
	}
	ut32 j = r_buf_tell (b);
	if (r_buf_tell (b) + ptr->body_size - 1 > bound) {
		goto beach;
	}
	if (!consume_u32_r (b, bound, &ptr->local_count)) {
		goto beach;
	}
	if (consume_locals_r (b, bound, ptr) < ptr->local_count) {
		goto beach;
	}
	ptr->code = r_buf_tell (b);
	ptr->len = ptr->body_size - ptr->code + j;
	r_buf_seek (b, ptr->len - 1, R_BUF_CUR); // consume bytecode
	ut8 end;
	r_buf_read (b, &end, 1);
	if (end != R_BIN_WASM_END_OF_CODE) {
		ut32 where = r_buf_tell (b) - 1;
		eprintf ("[wasm] Code entry at starting at 0x%x has ending byte 0x%x at 0x%x, should be 0x%x\n",
			(ut32)ptr->file_offset, end, where, R_BIN_WASM_END_OF_CODE);
		goto beach;
	}
	return ptr;

beach:
	free_code_entry (ptr);
	return NULL;
}