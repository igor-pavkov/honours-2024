static int r_core_cmd_subst(RCore *core, char *cmd) {
	ut64 rep = strtoull (cmd, NULL, 10);
	int ret = 0, orep;
	char *cmt, *colon = NULL, *icmd = NULL;
	bool tmpseek = false;
	bool original_tmpseek = core->tmpseek;

	if (r_str_startswith (cmd, "GET /cmd/")) {
		memmove (cmd, cmd + 9, strlen (cmd + 9) + 1);
		char *http = strstr (cmd, "HTTP");
		if (http) {
			*http = 0;
			http--;
			if (*http == ' ') {
				*http = 0;
			}
		}
		r_cons_printf ("HTTP/1.0 %d %s\r\n%s"
				"Connection: close\r\nContent-Length: %d\r\n\r\n",
				200, "OK", "", -1);
		return r_core_cmd0 (core, cmd);
	}

	/* must store a local orig_offset because there can be
	 * nested call of this function */
	ut64 orig_offset = core->offset;
	icmd = strdup (cmd);

	if (core->max_cmd_depth - core->cons->context->cmd_depth == 1) {
		core->prompt_offset = core->offset;
	}
	cmd = r_str_trim_head_tail (icmd);
	// lines starting with # are ignored (never reach cmd_hash()), except #! and #?
	if (!*cmd) {
		if (core->cmdrepeat > 0) {
			r_core_cmd_repeat (core, true);
			ret = r_core_cmd_nullcallback (core);
		}
		goto beach;
	}
	if (!icmd || (cmd[0] == '#' && cmd[1] != '!' && cmd[1] != '?')) {
		goto beach;
	}
	cmt = *icmd ? (char *)r_str_firstbut (icmd, '#', "\""): NULL;
	if (cmt && (cmt[1] == ' ' || cmt[1] == '\t')) {
		*cmt = 0;
	}
	if (*cmd != '"') {
		if (!strchr (cmd, '\'')) { // allow | awk '{foo;bar}' // ignore ; if there's a single quote
			if ((colon = strchr (cmd, ';'))) {
				*colon = 0;
			}
		}
	} else {
		colon = NULL;
	}
	if (rep > 0) {
		while (IS_DIGIT (*cmd)) {
			cmd++;
		}
		// do not repeat null cmd
		if (!*cmd) {
			goto beach;
		}
	}
	if (rep < 1) {
		rep = 1;
	}
	// XXX if output is a pipe then we don't want to be interactive
	if (rep > 1 && r_sandbox_enable (0)) {
		eprintf ("Command repeat sugar disabled in sandbox mode (%s)\n", cmd);
		goto beach;
	} else {
		if (rep > INTERACTIVE_MAX_REP) {
			if (r_cons_is_interactive ()) {
				if (!r_cons_yesno ('n', "Are you sure to repeat this %"PFMT64d" times? (y/N)", rep)) {
					goto beach;
				}
			}
		}
	}
	// TODO: store in core->cmdtimes to speedup ?
	const char *cmdrep = core->cmdtimes ? core->cmdtimes: "";
	orep = rep;

	r_cons_break_push (NULL, NULL);

	int ocur_enabled = core->print && core->print->cur_enabled;
	while (rep-- && *cmd) {
		if (core->print) {
			core->print->cur_enabled = false;
			if (ocur_enabled && core->seltab >= 0) {
				if (core->seltab == core->curtab) {
					core->print->cur_enabled = true;
				}
			}
		}
		if (r_cons_is_breaked ()) {
			break;
		}
		char *cr = strdup (cmdrep);
		core->break_loop = false;
		ret = r_core_cmd_subst_i (core, cmd, colon, (rep == orep - 1) ? &tmpseek : NULL);
		if (ret && *cmd == 'q') {
			free (cr);
			goto beach;
		}
		if (core->break_loop) {
			free (cr);
			break;
		}
		if (cr && *cr && orep > 1) {
			// XXX: do not flush here, we need r_cons_push () and r_cons_pop()
			r_cons_flush ();
			// XXX: we must import register flags in C
			(void)r_core_cmd0 (core, ".dr*");
			(void)r_core_cmd0 (core, cr);
		}
		free (cr);
	}

	r_cons_break_pop ();

	if (tmpseek) {
		r_core_seek (core, orig_offset, 1);
		core->tmpseek = original_tmpseek;
	}
	if (core->print) {
		core->print->cur_enabled = ocur_enabled;
	}
	if (colon && colon[1]) {
		for (++colon; *colon == ';'; colon++) {
			;
		}
		r_core_cmd_subst (core, colon);
	} else {
		if (!*icmd) {
			r_core_cmd_nullcallback (core);
		}
	}
beach:
	free (icmd);
	return ret;
}