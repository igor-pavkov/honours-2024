R_API int r_core_cmd_pipe(RCore *core, char *radare_cmd, char *shell_cmd) {
#if __UNIX__
	int stdout_fd, fds[2];
	int child;
#endif
	int si, olen, ret = -1, pipecolor = -1;
	char *str, *out = NULL;

	if (r_sandbox_enable (0)) {
		eprintf ("Pipes are not allowed in sandbox mode\n");
		return -1;
	}
	si = r_cons_is_interactive ();
	r_config_set_i (core->config, "scr.interactive", 0);
	if (!r_config_get_i (core->config, "scr.color.pipe")) {
		pipecolor = r_config_get_i (core->config, "scr.color");
		r_config_set_i (core->config, "scr.color", COLOR_MODE_DISABLED);
	}
	if (*shell_cmd=='!') {
		r_cons_grep_parsecmd (shell_cmd, "\"");
		olen = 0;
		out = NULL;
		// TODO: implement foo
		str = r_core_cmd_str (core, radare_cmd);
		r_sys_cmd_str_full (shell_cmd + 1, str, &out, &olen, NULL);
		free (str);
		r_cons_memcat (out, olen);
		free (out);
		ret = 0;
	}
#if __UNIX__
	radare_cmd = (char*)r_str_trim_head (radare_cmd);
	shell_cmd = (char*)r_str_trim_head (shell_cmd);

	signal (SIGPIPE, SIG_IGN);
	stdout_fd = dup (1);
	if (stdout_fd != -1) {
		if (pipe (fds) == 0) {
			child = r_sys_fork ();
			if (child == -1) {
				eprintf ("Cannot fork\n");
				close (stdout_fd);
			} else if (child) {
				dup2 (fds[1], 1);
				close (fds[1]);
				close (fds[0]);
				r_core_cmd (core, radare_cmd, 0);
				r_cons_flush ();
				close (1);
				wait (&ret);
				dup2 (stdout_fd, 1);
				close (stdout_fd);
			} else {
				close (fds[1]);
				dup2 (fds[0], 0);
				//dup2 (1, 2); // stderr goes to stdout
				r_sandbox_system (shell_cmd, 0);
				close (stdout_fd);
			}
		} else {
			eprintf ("r_core_cmd_pipe: Could not pipe\n");
		}
	}
#elif __WINDOWS__
	r_w32_cmd_pipe (core, radare_cmd, shell_cmd);
#else
#ifdef _MSC_VER
#pragma message ("r_core_cmd_pipe UNIMPLEMENTED FOR THIS PLATFORM")
#else
#warning r_core_cmd_pipe UNIMPLEMENTED FOR THIS PLATFORM
#endif
	eprintf ("r_core_cmd_pipe: unimplemented for this platform\n");
#endif
	if (pipecolor != -1) {
		r_config_set_i (core->config, "scr.color", pipecolor);
	}
	r_config_set_i (core->config, "scr.interactive", si);
	return ret;
}