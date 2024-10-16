R_API RBinClass *r_bin_file_add_class(RBinFile *bf, const char *name, const char *super, int view) {
	r_return_val_if_fail (name && bf && bf->o, NULL);
	RBinClass *c = __getClass (bf, name);
	if (c) {
		if (super) {
			free (c->super);
			c->super = strdup (super);
		}
		return c;
	}
	c = r_bin_class_new (name, super, view);
	if (c) {
		// XXX. no need for a list, the ht is iterable too
		c->index = r_list_length (bf->o->classes);
		r_list_append (bf->o->classes, c);
		ht_pp_insert (bf->o->classes_ht, name, c);
	}
	return c;
}