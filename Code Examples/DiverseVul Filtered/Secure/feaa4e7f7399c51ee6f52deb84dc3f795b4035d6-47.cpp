static void create_initterm_syms(RKext *kext, RList *ret, int type, ut64 *pointers) {
	int i = 0;
	int count = 0;
	for (; pointers[i]; i++) {
		ut64 func_vaddr = pointers[i];
		ut64 text_start = kext->vaddr;
		ut64 text_end = text_start + kext->text_range.size;

		if (text_start == text_end) {
			continue;
		}

		if (text_start > func_vaddr || func_vaddr >= text_end) {
			continue;
		}

		RBinSymbol *sym = R_NEW0 (RBinSymbol);
		if (!sym) {
			break;
		}

		sym->name = r_str_newf ("%s.%s.%d", kext_short_name (kext), (type == R_BIN_ENTRY_TYPE_INIT) ? "init" : "fini", count++);
		sym->vaddr = func_vaddr;
		sym->paddr = func_vaddr - kext->pa2va_exec;
		sym->size = 0;
		sym->forwarder = "NONE";
		sym->bind = "GLOBAL";
		sym->type = "FUNC";

		r_list_append (ret, sym);
	}
}