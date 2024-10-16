R_API char *r_egg_to_string(REgg *egg) {
	return r_buf_to_string (egg->buf);
}