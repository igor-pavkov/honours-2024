R_API RBinClass *r_bin_class_new(RBinFile *binfile, const char *name,
				  const char *super, int view) {
	RBinObject *o = binfile? binfile->o: NULL;
	RList *list = NULL;
	RBinClass *c;
	if (!o) {
		return NULL;
	}
	list = o->classes;
	if (!name) {
		return NULL;
	}
	c = r_bin_class_get (binfile, name);
	if (c) {
		if (super) {
			free (c->super);
			c->super = strdup (super);
		}
		return c;
	}
	c = R_NEW0 (RBinClass);
	if (!c) {
		return NULL;
	}
	c->name = strdup (name);
	c->super = super? strdup (super): NULL;
	c->index = r_list_length (list);
	c->methods = r_list_new ();
	c->fields = r_list_new ();
	c->visibility = view;
	if (!list) {
		list = o->classes = r_list_new ();
	}
	r_list_append (list, c);
	return c;
}