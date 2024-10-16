static void __get_symbols_at(r_bin_le_obj_t *bin, RList *syml, RList *entl, ut64 offset, ut64 end) {
	while (offset < end) {
		RBinSymbol *sym = __get_symbol (bin, &offset);
		if (!sym) {
			break;
		}
		if (sym->ordinal) {
			const char *n = r_list_get_n (entl, sym->ordinal - 1);
			if (n) {
				sym->vaddr = r_num_get (NULL, n);
				sym->bind = R_BIN_BIND_GLOBAL_STR;
				sym->type = R_BIN_TYPE_FUNC_STR;
				r_list_append (syml, sym);
			}
		} else {
			r_bin_symbol_free (sym);
		}
	}
}