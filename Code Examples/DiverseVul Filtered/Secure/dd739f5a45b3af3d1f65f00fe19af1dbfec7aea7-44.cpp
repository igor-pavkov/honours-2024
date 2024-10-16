static int cmd_uname(void *data, const char *input) {
	RCore *core = (RCore *)data;
	switch (input[0]) {
	case '?': // "u?"
		r_core_cmd_help (data, help_msg_u);
		return 1;
	case 'c': // "uc"
		switch (input[1]) {
		case ' ': {
			char *cmd = strdup (input + 2);
			char *rcmd = strchr (cmd, ',');
			if (rcmd) {
				*rcmd++ = 0;
				RCoreUndo *undo = r_core_undo_new (core->offset, cmd, rcmd);
				r_core_undo_push (core, undo);
			} else {
				eprintf ("Usage: uc [cmd] [revert-cmd]");
			}
			free (cmd);
			}
			break;
		case '?':
			eprintf ("Usage: uc [cmd],[revert-cmd]\n");
			eprintf (" uc. - list all reverts in current\n");
			eprintf (" uc* - list all core undos\n");
			eprintf (" uc  - list all core undos\n");
			eprintf (" uc- - undo last action\n");
			break;
		case '.': {
			RCoreUndoCondition cond = {
				.addr = core->offset,
				.minstamp = 0,
				.glob = NULL
			};
			r_core_undo_print (core, 1, &cond);
			} break;
		case '*':
			r_core_undo_print (core, 1, NULL);
			break;
		case '-': // "uc-"
			r_core_undo_pop (core);
			break;
		default:
			r_core_undo_print (core, 0, NULL);
			break;
		}
		return 1;
	case 's': // "us"
		r_core_cmdf (data, "s-%s", input + 1);
		return 1;
	case 'w': // "uw"
		r_core_cmdf (data, "wc%s", input + 1);
		return 1;
	case 'n': // "un"
		if (input[1] == 'i' && input[2] == 'q') {
			cmd_uniq (core, input);
		}
		return 1;
	}
#if __UNIX__
	struct utsname un;
	uname (&un);
	r_cons_printf ("%s %s %s %s\n", un.sysname,
		un.nodename, un.release, un.machine);
#elif __WINDOWS__
	r_cons_printf ("windows\n");
#else
	r_cons_printf ("unknown\n");
#endif
	return 0;
}