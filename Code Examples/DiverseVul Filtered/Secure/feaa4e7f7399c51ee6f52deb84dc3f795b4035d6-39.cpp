static void symbols_from_mach0(RList *ret, struct MACH0_(obj_t) *mach0, RBinFile *bf, ut64 paddr, int ordinal) {
	const struct symbol_t *symbols = MACH0_(get_symbols) (mach0);
	if (!symbols) {
		return;
	}
	int i;
	for (i = 0; !symbols[i].last; i++) {
		if (!symbols[i].name[0] || symbols[i].addr < 100) {
			continue;
		}
		RBinSymbol *sym = R_NEW0 (RBinSymbol);
		if (!sym) {
			break;
		}
		sym->name = strdup (symbols[i].name);
		sym->vaddr = symbols[i].addr;
		if (sym->name[0] == '_') {
			char *dn = r_bin_demangle (bf, sym->name, sym->name, sym->vaddr, false);
			if (dn) {
				sym->dname = dn;
				char *p = strchr (dn, '.');
				if (p) {
					if (IS_UPPER (sym->name[0])) {
						sym->classname = strdup (sym->name);
						sym->classname[p - sym->name] = 0;
					} else if (IS_UPPER (p[1])) {
						sym->classname = strdup (p + 1);
						p = strchr (sym->classname, '.');
						if (p) {
							*p = 0;
						}
					}
				}
			}
		}
		sym->forwarder = "NONE";
		sym->bind = (symbols[i].type == R_BIN_MACH0_SYMBOL_TYPE_LOCAL)? "LOCAL": "GLOBAL";
		sym->type = "FUNC";
		sym->paddr = symbols[i].offset + bf->o->boffset + paddr;
		sym->size = symbols[i].size;
		sym->ordinal = ordinal + i;
		r_list_append (ret, sym);
	}
}