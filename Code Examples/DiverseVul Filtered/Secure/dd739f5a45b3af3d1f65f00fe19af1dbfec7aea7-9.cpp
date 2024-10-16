R_API char *r_core_cmd_str(RCore *core, const char *cmd) {
	const char *static_str;
	char *retstr = NULL;
	r_cons_push ();
	if (r_core_cmd (core, cmd, 0) == -1) {
		//eprintf ("Invalid command: %s\n", cmd);
		return NULL;
	}
	r_cons_filter ();
	static_str = r_cons_get_buffer ();
	retstr = strdup (static_str? static_str: "");
	r_cons_pop ();
	r_cons_echo (NULL);
	return retstr;
}