R_IPI void r_bin_class_free(RBinClass *k) {
	if (k) {
		free (k->name);
		free (k->super);
		free (k->visibility_str);
		r_list_free (k->methods);
		r_list_free (k->fields);
		free (k);
	}
}