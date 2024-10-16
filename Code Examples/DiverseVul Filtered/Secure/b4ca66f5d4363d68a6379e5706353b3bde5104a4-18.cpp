static RBinWasmTableEntry *parse_table_entry(RBinWasmObj *bin, ut64 bound, ut32 index) {
	RBuffer *b = bin->buf;
	RBinWasmTableEntry *table = R_NEW0 (RBinWasmTableEntry);
	if (table) {
		table->sec_i = index;
		table->file_offset = r_buf_tell (b);
		if (!consume_s7_r (b, bound, (st8 *)&table->element_type)) {
			goto beach;
		}
		if (!consume_limits_r (b, bound, &table->limits)) {
			goto beach;
		}
	}
	return table;

beach:
	free (table);
	return NULL;
}