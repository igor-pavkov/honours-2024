static int cmd_env(void *data, const char *input) {
	RCore *core = (RCore*)data;
	int ret = true;
	switch (*input) {
	case '?':
		cmd_help_percent (core);
		break;
	default:
		ret = r_core_cmdf (core, "env %s", input);
	}
	return ret;
}