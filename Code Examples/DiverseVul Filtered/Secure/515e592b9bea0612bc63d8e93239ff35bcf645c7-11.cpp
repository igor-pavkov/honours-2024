static RBinSymbol *bin_symbol_from_symbol(RCoreSymCacheElement *element, RCoreSymCacheElementSymbol *s) {
	if (!s->name && !s->mangled_name) {
		return NULL;
	}
	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (sym) {
		if (s->name && s->mangled_name) {
			sym->dname = strdup (s->name);
			sym->name = strdup (s->mangled_name);
		} else if (s->name) {
			sym->name = strdup (s->name);
		} else if (s->mangled_name) {
			sym->name = s->mangled_name;
		}
		sym->paddr = s->paddr;
		sym->vaddr = r_coresym_cache_element_pa2va (element, s->paddr);
		sym->size = s->size;
		sym->type = R_BIN_TYPE_FUNC_STR;
		sym->bind = "NONE";
	}
	return sym;
}