R_API void r_egg_printf(REgg *egg, const char *fmt, ...) {
	va_list ap;
	int len;
	char buf[1024];
	va_start (ap, fmt);
	len = vsnprintf (buf, sizeof (buf), fmt, ap);
	r_buf_append_bytes (egg->buf, (const ut8 *)buf, len);
	va_end (ap);
}