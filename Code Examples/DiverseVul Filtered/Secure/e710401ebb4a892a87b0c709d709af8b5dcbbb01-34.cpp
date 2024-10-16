R_API void r_egg_pattern(REgg *egg, int size) {
	char *ret = r_debruijn_pattern ((int)size, 0, NULL);
	if (ret) {
		r_egg_prepend_bytes (egg, (const ut8*)ret, strlen(ret));
		free (ret);
	} else {
		eprintf ("Invalid debruijn pattern length.\n");
	}
}