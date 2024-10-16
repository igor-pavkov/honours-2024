R_IPI RBinClass *r_bin_class_new(const char *name, const char *super, int view) {
	r_return_val_if_fail (name, NULL);
	RBinClass *c = R_NEW0 (RBinClass);
	if (c) {
		c->name = strdup (name);
		c->super = super? strdup (super): NULL;
		c->methods = r_list_new ();
		c->fields = r_list_new ();
		c->visibility = view;
	}
	return c;
}