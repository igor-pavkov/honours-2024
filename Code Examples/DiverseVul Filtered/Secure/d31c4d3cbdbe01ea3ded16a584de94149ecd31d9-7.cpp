R_API void r_bin_class_free(RBinClass *c) {
	free (c->name);
	free (c->super);
	r_list_free (c->methods);
	r_list_free (c->fields);
	free (c);
}