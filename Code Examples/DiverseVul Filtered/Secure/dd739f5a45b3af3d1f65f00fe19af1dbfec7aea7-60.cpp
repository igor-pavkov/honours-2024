R_API int r_core_cmd_buffer(RCore *core, const char *buf) {
	char *ptr, *optr, *str = strdup (buf);
	if (!str) {
		return false;
	}
	optr = str;
	ptr = strchr (str, '\n');
	while (ptr) {
		*ptr = '\0';
		r_core_cmd (core, optr, 0);
		optr = ptr + 1;
		ptr = strchr (str, '\n');
	}
	r_core_cmd (core, optr, 0);
	free (str);
	return true;
}