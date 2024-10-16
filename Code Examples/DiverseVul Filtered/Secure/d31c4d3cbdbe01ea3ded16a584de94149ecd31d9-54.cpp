R_API RBinSymbol *r_bin_class_add_method(RBinFile *binfile, const char *classname, const char *name, int nargs) {
	RBinClass *c = r_bin_class_get (binfile, classname);
	if (!c) {
		c = r_bin_class_new (binfile, classname, NULL, 0);
		if (!c) {
			eprintf ("Cannot allocate class %s\n", classname);
			return NULL;
		}
	}
	RBinSymbol *m;
	RListIter *iter;
	r_list_foreach (c->methods, iter, m) {
		if (!strcmp (m->name, name)) {
			return NULL;
		}
	}
	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (!sym) {
		return NULL;
	}
	sym->name = strdup (name);
	r_list_append (c->methods, sym);
	return sym;
}