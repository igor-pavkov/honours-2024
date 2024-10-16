static int cmd_visual(void *data, const char *input) {
	RCore *core = (RCore*) data;
	if (core->http_up) {
		return false;
	}
	if (!r_cons_is_interactive ()) {
		return false;
	}
	return r_core_visual ((RCore *)data, input);
}