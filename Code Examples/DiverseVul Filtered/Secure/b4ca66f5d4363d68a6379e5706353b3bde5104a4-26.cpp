static RBinWasmFunctionEntry *parse_function_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmFunctionEntry *func = R_NEW0 (RBinWasmFunctionEntry);
	if (func && consume_u32_r (b, bound, &func->typeindex)) {
		func->sec_i = index;
		func->file_offset = r_buf_tell (b);
		return func;
	}
	free (func);
	return NULL;
}