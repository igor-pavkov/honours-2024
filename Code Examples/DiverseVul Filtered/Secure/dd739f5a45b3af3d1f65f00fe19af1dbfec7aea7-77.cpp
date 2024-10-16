R_API int r_core_cmdf(RCore *core, const char *fmt, ...) {
	char string[4096];
	int ret;
	va_list ap;
	va_start (ap, fmt);
	vsnprintf (string, sizeof (string), fmt, ap);
	ret = r_core_cmd (core, string, 0);
	va_end (ap);
	return ret;
}