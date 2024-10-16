static RBinWasmMemoryEntry *parse_memory_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmMemoryEntry *ptr = R_NEW0 (RBinWasmMemoryEntry);
	if (ptr) {
		ptr->sec_i = index;
		ptr->file_offset = r_buf_tell (b);
		if (!consume_limits_r (b, bound, &ptr->limits)) {
			free (ptr);
			return NULL;
		}
	}
	return ptr;
}