static void r_bin_object_delete_items(RBinObject *o) {
	ut32 i = 0;
	if (!o) {
		return;
	}
	r_list_free (o->entries);
	r_list_free (o->fields);
	r_list_free (o->imports);
	r_list_free (o->libs);
	r_list_free (o->relocs);
	r_list_free (o->sections);
	r_list_free (o->strings);
	r_list_free (o->symbols);
	r_list_free (o->classes);
	r_list_free (o->lines);
	sdb_free (o->kv);
	if (o->mem) {
		o->mem->free = mem_free;
	}
	r_list_free (o->mem);
	o->mem = NULL;
	o->entries = NULL;
	o->fields = NULL;
	o->imports = NULL;
	o->libs = NULL;
	o->relocs = NULL;
	o->sections = NULL;
	o->strings = NULL;
	o->symbols = NULL;
	o->classes = NULL;
	o->lines = NULL;
	o->info = NULL;
	o->kv = NULL;
	for (i = 0; i < R_BIN_SYM_LAST; i++) {
		free (o->binsym[i]);
		o->binsym[i] = NULL;
	}
}