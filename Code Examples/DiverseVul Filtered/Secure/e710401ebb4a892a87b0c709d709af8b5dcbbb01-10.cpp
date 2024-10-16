R_API int r_egg_include(REgg *egg, const char *file, int format) {
	int sz;
	const ut8 *foo = (const ut8 *)r_file_slurp (file, &sz);
	if (!foo) {
		return 0;
	}
	// XXX: format breaks compiler layers
	switch (format) {
	case 'r': // raw
		r_egg_raw (egg, foo, sz);
		break;
	case 'a': // assembly
		r_buf_append_bytes (egg->buf, foo, sz);
		break;
	default:
		r_buf_append_bytes (egg->src, foo, sz);
	}
	free ((void *)foo);
	return 1;
}