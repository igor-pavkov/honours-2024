static RBinSymbol *__get_symbol(r_bin_le_obj_t *bin, ut64 *offset) {
	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (!sym) {
		return NULL;
	}
	char *name = __read_nonnull_str_at (bin->buf, offset);
	if (!name) {
		r_bin_symbol_free (sym);
		return NULL;
	}
	sym->name = name;
	ut16 entry_idx = r_buf_read_le16_at (bin->buf, *offset);
	*offset += 2;
	sym->ordinal = entry_idx;
	return sym;
}