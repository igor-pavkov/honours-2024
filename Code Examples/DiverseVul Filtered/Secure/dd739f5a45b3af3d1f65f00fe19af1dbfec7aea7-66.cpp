static int cmd_uniq(void *data, const char *input) { // "uniq"
	RCore *core = (RCore *)data;
	const char *arg = strchr (input, ' ');
	if (arg) {
		arg = r_str_trim_ro (arg + 1);
	}
	switch (*input) {
	case '?': // "uniq?"
		eprintf ("Usage: uniq # uniq to list unique strings in file\n");
		break;
	default: // "uniq"
		if (!arg) {
			arg = "";
		}
		if (r_fs_check (core->fs, arg)) {
			r_core_cmdf (core, "md %s", arg);
		} else {
			char *res = r_syscmd_uniq (arg);
			if (res) {
				r_cons_print (res);
				free (res);
			}
		}
		break;
	}
	return 0;
}