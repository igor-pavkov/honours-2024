R_API void r_egg_append(REgg *egg, const char *src) {
	r_buf_append_bytes (egg->src, (const ut8*)src, strlen (src));
}