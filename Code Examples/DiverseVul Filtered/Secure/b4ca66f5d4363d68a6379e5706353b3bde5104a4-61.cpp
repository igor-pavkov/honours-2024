static inline RPVector *parse_vec(RBinWasmObj *bin, ut64 bound, ParseEntryFcn parse_entry, RPVectorFree free_entry) {
	RBuffer *buf = bin->buf;

	ut32 count;
	if (!consume_u32_r (buf, bound, &count)) {
		return NULL;
	}

	RPVector *vec = r_pvector_new (free_entry);
	if (vec) {
		r_pvector_reserve (vec, count);
		ut32 i;
		for (i = 0; i < count; i++) {
			ut64 start = r_buf_tell (buf);
			void *e = parse_entry (bin, bound, i);
			if (!e || !r_pvector_push (vec, e)) {
				eprintf ("[wasm] Failed to parse entry %u/%u of vec at 0x%" PFMT64x "\n", i, count, start);
				free_entry (e);
				break;
			}
		}
	}
	return vec;
}