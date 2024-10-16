R_API RList *r_bin_classes_from_symbols (RBinFile *bf, RBinObject *o) {
	RBinSymbol *sym;
	RListIter *iter;
	RList *symbols = o->symbols;
	RList *classes = o->classes;
	if (!classes) {
		classes = r_list_newf ((RListFree)r_bin_class_free);
	}
	r_list_foreach (symbols, iter, sym) {
		if (sym->name[0] != '_') {
			continue;
		}
		const char *cn = sym->classname;
		if (cn) {
			RBinClass *c = r_bin_class_new (bf, sym->classname, NULL, 0);
			if (!c) {
				continue;
			}
			// swift specific
			char *dn = sym->dname;
			char *fn = swiftField (dn, cn);
			if (fn) {
				// eprintf ("FIELD %s  %s\n", cn, fn);
				RBinField *f = r_bin_field_new (sym->paddr, sym->vaddr, sym->size, fn, NULL, NULL);
				r_list_append (c->fields, f);
				free (fn);
			} else {
				char *mn = strstr (dn, "..");
				if (mn) {
					// eprintf ("META %s  %s\n", sym->classname, mn);
				} else {
					char *mn = strstr (dn, cn);
					if (mn && mn[strlen(cn)] == '.') {
						mn += strlen (cn) + 1;
						// eprintf ("METHOD %s  %s\n", sym->classname, mn);
						r_list_append (c->methods, sym);
					}
				}
			}
		}
	}
	if (r_list_empty (classes)) {
		r_list_free (classes);
		return NULL;
	}
	return classes;
}