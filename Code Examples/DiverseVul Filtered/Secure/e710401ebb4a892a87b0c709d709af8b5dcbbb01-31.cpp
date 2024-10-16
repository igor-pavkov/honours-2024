R_API int r_egg_encode(REgg *egg, const char *name) {
	REggPlugin *p;
	RListIter *iter;
	RBuffer *b;
	r_list_foreach (egg->plugins, iter, p) {
		if (p->type == R_EGG_PLUGIN_ENCODER && !strcmp (name, p->name)) {
			b = p->build (egg);
			if (!b) {
				return false;
			}
			r_buf_free (egg->bin);
			egg->bin = b;
			return true;
		}
	}
	return false;
}