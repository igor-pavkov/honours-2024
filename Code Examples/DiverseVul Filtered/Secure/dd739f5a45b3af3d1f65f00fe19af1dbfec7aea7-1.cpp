static int cmd_pointer(void *data, const char *input) {
	RCore *core = (RCore*) data;
	int ret = true;
	char *str, *eq;
	input = r_str_trim_ro (input);
	while (*input == ' ') {
		input++;
	}
	if (!*input || *input == '?') {
		r_core_cmd_help (core, help_msg_star);
		return ret;
	}
	str = strdup (input);
	eq = strchr (str, '=');
	if (eq) {
		*eq++ = 0;
		if (!strncmp (eq, "0x", 2)) {
			ret = r_core_cmdf (core, "wv %s@%s", eq, str);
		} else {
			ret = r_core_cmdf (core, "wx %s@%s", eq, str);
		}
	} else {
		ret = r_core_cmdf (core, "?v [%s]", input);
	}
	free (str);
	return ret;
}