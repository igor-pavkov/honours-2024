R_API int r_egg_add(REgg *a, REggPlugin *foo) {
	RListIter *iter;
	RAsmPlugin *h;
	// TODO: cache foo->name length and use memcmp instead of strcmp
	if (!foo->name) {
		return false;
	}
	r_list_foreach (a->plugins, iter, h) {
		if (!strcmp (h->name, foo->name)) {
			return false;
		}
	}
	r_list_append (a->plugins, foo);
	return true;
}