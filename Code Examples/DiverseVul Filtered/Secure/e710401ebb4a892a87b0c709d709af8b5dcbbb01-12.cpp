R_API int r_egg_shellcode(REgg *egg, const char *name) {
	REggPlugin *p;
	RListIter *iter;
	RBuffer *b;
	r_list_foreach (egg->plugins, iter, p) {
		if (p->type == R_EGG_PLUGIN_SHELLCODE && !strcmp (name, p->name)) {
			b = p->build (egg);
			if (!b) {
				eprintf ("%s Shellcode has failed\n", p->name);
				return false;
			}
			ut64 tmpsz;
			const ut8 *tmp = r_buf_data (b, &tmpsz);
			r_egg_raw (egg, tmp, tmpsz);
			return true;
		}
	}
	return false;
}