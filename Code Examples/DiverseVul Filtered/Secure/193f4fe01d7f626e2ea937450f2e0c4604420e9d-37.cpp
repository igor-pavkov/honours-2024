R_API RBinSymbol *r_bin_file_add_method(RBinFile *bf, const char *klass, const char *method, int nargs) {
	r_return_val_if_fail (bf, NULL);

	RBinClass *c = r_bin_file_add_class (bf, klass, NULL, 0);
	if (!c) {
		eprintf ("Cannot allocate class %s\n", klass);
		return NULL;
	}
	RBinSymbol *sym = __getMethod (bf, klass, method);
	if (!sym) {
		sym = R_NEW0 (RBinSymbol);
		if (sym) {
			sym->name = strdup (method);
			r_list_append (c->methods, sym);
			char *name = r_str_newf ("%s::%s", klass, method);
			ht_pp_insert (bf->o->methods_ht, name, sym);
			free (name);
		}
	}
	return sym;
}