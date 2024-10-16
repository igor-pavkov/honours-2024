static bool cmd_r2cmd(RCore *core, const char *_input) {
	char *input = r_str_newf ("r%s", _input);
	int rc = 0;
	if (r_str_startswith (input, "rax2")) {
		rc = __runMain (core->r_main_rax2, input);
	} else if (r_str_startswith (input, "radare2")) {
		r_sys_cmdf ("%s", input);
		// rc = __runMain (core->r_main_radare2, input);
	} else if (r_str_startswith (input, "rasm2")) {
		r_sys_cmdf ("%s", input);
		// rc = __runMain (core->r_main_rasm2, input);
	} else if (r_str_startswith (input, "rabin2")) {
		r_sys_cmdf ("%s", input);
		// rc = __runMain (core->r_main_rabin2, input);
	} else if (r_str_startswith (input, "ragg2")) {
		r_sys_cmdf ("%s", input);
		// rc = __runMain (core->r_main_ragg2, input);
	} else if (r_str_startswith (input, "r2pm")) {
		r_sys_cmdf ("%s", input);
		// rc = __runMain (core->r_main_r2pm, input);
	} else if (r_str_startswith (input, "radiff2")) {
		rc = __runMain (core->r_main_radiff2, input);
	} else {
		const char *r2cmds[] = {
			"rax2", "r2pm", "rasm2", "rabin2", "rahash2", "rafind2", "rarun2", "ragg2", "radare2", "r2", NULL
		};
		int i;
		for (i = 0; r2cmds[i]; i++) {
			if (r_str_startswith (input, r2cmds[i])) {
				free (input);
				return true;
			}
		}
		return false;
	}
	free (input);
	core->num->value = rc;
	return true;
}