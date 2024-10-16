R_API char *r_core_cmd_str_pipe(RCore *core, const char *cmd) {
	char *s, *tmp = NULL;
	if (r_sandbox_enable (0)) {
		char *p = (*cmd != '"')? strchr (cmd, '|'): NULL;
		if (p) {
			// This code works but its pretty ugly as its a workaround to
			// make the webserver work as expected, this was broken some
			// weeks. let's use this hackaround for now
			char *c = strdup (cmd);
			c[p - cmd] = 0;
			if (!strcmp (p + 1, "H")) {
				char *res = r_core_cmd_str (core, c);
				free (c);
				char *hres = r_cons_html_filter (res, NULL);
				free (res);
				return hres;
			} else {
				int sh = r_config_get_i (core->config, "scr.color");
				r_config_set_i (core->config, "scr.color", 0);
				char *ret = r_core_cmd_str (core, c);
				r_config_set_i (core->config, "scr.color", sh);
				free (c);
				return ret;
			}
		}
		return r_core_cmd_str (core, cmd);
	}
	r_cons_reset ();
	r_sandbox_disable (1);
	if (r_file_mkstemp ("cmd", &tmp) != -1) {
		int pipefd = r_cons_pipe_open (tmp, 1, 0);
		if (pipefd == -1) {
			r_file_rm (tmp);
			r_sandbox_disable (0);
			free (tmp);
			return r_core_cmd_str (core, cmd);
		}
		char *_cmd = strdup (cmd);
		r_core_cmd_subst (core, _cmd);
		r_cons_flush ();
		r_cons_pipe_close (pipefd);
		s = r_file_slurp (tmp, NULL);
		if (s) {
			r_file_rm (tmp);
			r_sandbox_disable (0);
			free (tmp);
			free (_cmd);
			return s;
		}
		eprintf ("slurp %s fails\n", tmp);
		r_file_rm (tmp);
		free (tmp);
		free (_cmd);
		r_sandbox_disable (0);
		return r_core_cmd_str (core, cmd);
	}
	r_sandbox_disable (0);
	return NULL;
}