R_API RBinClass *r_bin_class_get(RBinFile *binfile, const char *name) {
	if (!binfile || !binfile->o || !name) {
		return NULL;
	}
	RBinClass *c;
	RListIter *iter;
	RList *list = binfile->o->classes;
	r_list_foreach (list, iter, c) {
		if (!strcmp (c->name, name)) {
			return c;
		}
	}
	return NULL;
}