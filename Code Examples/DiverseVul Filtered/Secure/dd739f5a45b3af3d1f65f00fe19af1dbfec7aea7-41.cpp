static int cmd_alias(void *data, const char *input) {
	RCore *core = (RCore *)data;
	if (*input == '?') {
		r_core_cmd_help (core, help_msg_dollar);
		return 0;
	}
	int i = strlen (input);
	char *buf = malloc (i + 2);
	if (!buf) {
		return 0;
	}
	*buf = '$'; // prefix aliases with a dollar
	memcpy (buf + 1, input, i + 1);
	char *q = strchr (buf, ' ');
	char *def = strchr (buf, '=');
	char *desc = strchr (buf, '?');

	/* create alias */
	if ((def && q && (def < q)) || (def && !q)) {
		*def++ = 0;
		size_t len = strlen (def);
		/* Remove quotes */
		if (len > 0 && (def[0] == '\'') && (def[len - 1] == '\'')) {
			def[len - 1] = 0x00;
			def++;
		}
		if (!q || (q && q > def)) {
			if (*def) {
				if (!strcmp (def, "-")) {
					char *v = r_cmd_alias_get (core->rcmd, buf, 0);
					char *n = r_cons_editor (NULL, v);
					if (n) {
						r_cmd_alias_set (core->rcmd, buf, n, 0);
						free (n);
					}
				} else {
					r_cmd_alias_set (core->rcmd, buf, def, 0);
				}
			} else {
				r_cmd_alias_del (core->rcmd, buf);
			}
		}
	/* Show command for alias */
	} else if (desc && !q) {
		*desc = 0;
		char *v = r_cmd_alias_get (core->rcmd, buf, 0);
		if (v) {
			r_cons_println (v);
			free (buf);
			return 1;
		} else {
			eprintf ("unknown key '%s'\n", buf);
		}
	} else if (buf[1] == '*') {
		/* Show aliases */
		int i, count = 0;
		char **keys = r_cmd_alias_keys (core->rcmd, &count);
		for (i = 0; i < count; i++) {
			char *v = r_cmd_alias_get (core->rcmd, keys[i], 0);
			char *q = r_base64_encode_dyn (v, -1);
			if (buf[2] == '*') {
				r_cons_printf ("%s=%s\n", keys[i], v);
			} else {
				r_cons_printf ("%s=base64:%s\n", keys[i], q);
			}
			free (q);
		}
	} else if (!buf[1]) {
		int i, count = 0;
		char **keys = r_cmd_alias_keys (core->rcmd, &count);
		for (i = 0; i < count; i++) {
			r_cons_println (keys[i]);
		}
	} else {
		/* Execute alias */
		if (q) {
			*q = 0;
		}
		char *v = r_cmd_alias_get (core->rcmd, buf, 0);
		if (v) {
			if (*v == '$') {
				r_cons_strcat (v + 1);
				r_cons_newline ();
			} else if (q) {
				char *out = r_str_newf ("%s %s", v, q + 1);
				r_core_cmd0 (core, out);
				free (out);
			} else {
				r_core_cmd0 (core, v);
			}
		} else {
			eprintf ("unknown key '%s'\n", buf);
		}
	}
	free (buf);
	return 0;
}