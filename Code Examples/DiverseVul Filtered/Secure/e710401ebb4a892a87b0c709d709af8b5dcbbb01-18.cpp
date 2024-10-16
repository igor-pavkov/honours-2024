R_API char *r_egg_get_source(REgg *egg) {
	return r_buf_to_string (egg->src);
}