R_API int r_core_cmd(RCore *core, const char *cstr, int log) {
	char *cmd, *ocmd, *ptr, *rcmd;
	int ret = false, i;

	if (core->cmdfilter) {
		const char *invalid_chars = ";|>`@";
		for (i = 0; invalid_chars[i]; i++) {
			if (strchr (cstr, invalid_chars[i])) {
				ret = true;
				goto beach;
			}
		}
		if (strncmp (cstr, core->cmdfilter, strlen (core->cmdfilter))) {
			ret = true;
			goto beach;
		}
	}
	if (core->cmdremote) {
		if (*cstr != '=' && *cstr != 'q' && strncmp (cstr, "!=", 2)) {
			char *res = r_io_system (core->io, cstr);
			if (res) {
				r_cons_printf ("%s\n", res);
				free (res);
			}
			goto beach; // false
		}
	}

	if (!cstr || (*cstr == '|' && cstr[1] != '?')) {
		// raw comment syntax
		goto beach; // false;
	}
	if (!strncmp (cstr, "/*", 2)) {
		if (r_sandbox_enable (0)) {
			eprintf ("This command is disabled in sandbox mode\n");
			goto beach; // false
		}
		core->incomment = true;
	} else if (!strncmp (cstr, "*/", 2)) {
		core->incomment = false;
		goto beach; // false
	}
	if (core->incomment) {
		goto beach; // false
	}
	if (log && (*cstr && (*cstr != '.' || !strncmp (cstr, ".(", 2)))) {
		free (core->lastcmd);
		core->lastcmd = strdup (cstr);
	}

	ocmd = cmd = malloc (strlen (cstr) + 4096);
	if (!ocmd) {
		goto beach;
	}
	r_str_cpy (cmd, cstr);
	if (log) {
		r_line_hist_add (cstr);
	}

	if (core->cons->context->cmd_depth < 1) {
		eprintf ("r_core_cmd: That was too deep (%s)...\n", cmd);
		free (ocmd);
		R_FREE (core->oobi);
		core->oobi_len = 0;
		goto beach;
	}
	core->cons->context->cmd_depth--;
	for (rcmd = cmd;;) {
		ptr = strchr (rcmd, '\n');
		if (ptr) {
			*ptr = '\0';
		}
		ret = r_core_cmd_subst (core, rcmd);
		if (ret == -1) {
			eprintf ("|ERROR| Invalid command '%s' (0x%02x)\n", rcmd, *rcmd);
			break;
		}
		if (!ptr) {
			break;
		}
		rcmd = ptr + 1;
	}
	/* run pending analysis commands */
	run_pending_anal (core);
	core->cons->context->cmd_depth++;
	free (ocmd);
	R_FREE (core->oobi);
	core->oobi_len = 0;
	return ret;
beach:
	if (r_list_empty (core->tasks)) {
		r_th_lock_leave (core->lock);
	} else {
		RListIter *iter;
		RCoreTask *task;
		r_list_foreach (core->tasks, iter, task) {
			r_th_pause (task->thread, false);
		}
	}
	/* run pending analysis commands */
	run_pending_anal (core);
	return ret;
}