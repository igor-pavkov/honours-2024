static int cmd_ls(void *data, const char *input) { // "ls"
	RCore *core = (RCore *)data;
	const char *arg = strchr (input, ' ');
	if (arg) {
		arg = r_str_trim_ro (arg + 1);
	}
	switch (*input) {
	case '?': // "l?"
		eprintf ("Usage: l[es] # ls to list files, le[ss] to less a file\n");
		break;
	case 'e': // "le"
		if (arg) {
			r_core_cmdf (core, "cat %s~..", arg);
		} else {
			eprintf ("Usage: less [file]\n");
		}
		break;
	default: // "ls"
		if (!arg) {
			arg = "";
		}
		if (r_fs_check (core->fs, arg)) {
			r_core_cmdf (core, "md %s", arg);
		} else {
			char *res = r_syscmd_ls (arg);
			if (res) {
				r_cons_print (res);
				free (res);
			}
		}
		break;
	}
	return 0;
}