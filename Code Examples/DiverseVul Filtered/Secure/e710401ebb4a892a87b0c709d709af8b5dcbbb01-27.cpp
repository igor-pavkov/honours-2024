R_API char *r_egg_get_assembly(REgg *egg) {
	return r_buf_to_string (egg->buf);
}