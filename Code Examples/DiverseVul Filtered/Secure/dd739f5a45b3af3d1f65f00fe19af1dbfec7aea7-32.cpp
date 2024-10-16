static int cmd_rap_run(void *data, const char *input) {
	RCore *core = (RCore *)data;
	char *res = r_io_system (core->io, input);
	if (res) {
		int ret = atoi (res);
		free (res);
		return ret;
	}
	return false;
}