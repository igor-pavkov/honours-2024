static int cmd_system(void *data, const char *input) {
	RCore *core = (RCore*)data;
	ut64 n;
	int ret = 0;
	switch (*input) {
	case '-': //!-
		if (input[1]) {
			r_line_hist_free();
			r_line_hist_save (R2_HOME_HISTORY);
		} else {
			r_line_hist_free();
		}
		break;
	case '=': //!=
		if (input[1] == '?') {
			r_cons_printf ("Usage: !=[!]  - enable/disable remote commands\n");
		} else {
			if (!r_sandbox_enable (0)) {
				core->cmdremote = input[1]? 1: 0;
				r_cons_println (r_str_bool (core->cmdremote));
			}
		}
		break;
	case '!': //!!
		if (input[1] == '!') { // !!! & !!!-
			cmd_autocomplete (core, input + 2);
		} else if (input[1] == '?') {
			cmd_help_exclamation (core);
		} else if (input[1] == '*') {
			char *cmd = r_str_trim_dup (input + 1);
			(void)r_core_cmdf (core, "\"#!pipe %s\"", cmd);
			free (cmd);
		} else {
			if (r_sandbox_enable (0)) {
				eprintf ("This command is disabled in sandbox mode\n");
				return 0;
			}
			if (input[1]) {
				int olen;
				char *out = NULL;
				char *cmd = r_core_sysenv_begin (core, input);
				if (cmd) {
					void *bed = r_cons_sleep_begin ();
					ret = r_sys_cmd_str_full (cmd + 1, NULL, &out, &olen, NULL);
					r_cons_sleep_end (bed);
					r_core_sysenv_end (core, input);
					r_cons_memcat (out, olen);
					free (out);
					free (cmd);
				} //else eprintf ("Error setting up system environment\n");
			} else {
				eprintf ("History saved to "R2_HOME_HISTORY"\n");
				r_line_hist_save (R2_HOME_HISTORY);
			}
		}
		break;
	case '\0':
		r_line_hist_list ();
		break;
	case '?': //!?
		cmd_help_exclamation (core);
		break;
	case '*':
		// TODO: use the api
		{
		char *cmd = r_str_trim_dup (input + 1);
		cmd = r_str_replace (cmd, " ", "\\ ", true);
		cmd = r_str_replace (cmd, "\\ ", " ", false);
		cmd = r_str_replace (cmd, "\"", "'", false);
		ret = r_core_cmdf (core, "\"#!pipe %s\"", cmd);
		free (cmd);
		}
		break;
	default:
		n = atoi (input);
		if (*input == '0' || n > 0) {
			const char *cmd = r_line_hist_get (n);
			if (cmd) {
				r_core_cmd0 (core, cmd);
			}
			//else eprintf ("Error setting up system environment\n");
		} else {
			char *cmd = r_core_sysenv_begin (core, input);
			if (cmd) {
				void *bed = r_cons_sleep_begin ();
				ret = r_sys_cmd (cmd);
				r_cons_sleep_end (bed);
				r_core_sysenv_end (core, input);
				free (cmd);
			} else {
				eprintf ("Error setting up system environment\n");
			}
		}
		break;
	}
	return ret;
}