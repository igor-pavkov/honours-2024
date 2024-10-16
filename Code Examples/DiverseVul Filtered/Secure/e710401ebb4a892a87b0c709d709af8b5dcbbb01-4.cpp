R_API void r_egg_load(REgg *egg, const char *code, int format) {
	switch (format) {
	case 'a': // assembly
		r_buf_append_bytes (egg->buf, (const ut8 *)code, strlen (code));
		break;
	default:
		r_buf_append_bytes (egg->src, (const ut8 *)code, strlen (code));
		break;
	}
}