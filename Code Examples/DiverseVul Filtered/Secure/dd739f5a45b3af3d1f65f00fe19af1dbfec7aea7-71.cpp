R_API int r_core_flush(RCore *core, const char *cmd) {
	int ret = r_core_cmd (core, cmd, 0);
	r_cons_flush ();
	return ret;
}