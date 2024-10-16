R_API int r_core_cmd_command(RCore *core, const char *command) {
	int ret, len;
	char *buf, *rcmd, *ptr;
	char *cmd = r_core_sysenv_begin (core, command);
	rcmd = ptr = buf = r_sys_cmd_str (cmd, 0, &len);
	if (!buf) {
		free (cmd);
		return -1;
	}
	ret = r_core_cmd (core, rcmd, 0);
	r_core_sysenv_end (core, command);
	free (buf);
	return ret;
}