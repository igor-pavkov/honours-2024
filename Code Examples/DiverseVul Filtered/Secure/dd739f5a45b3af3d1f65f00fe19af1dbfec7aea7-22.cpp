static char *parse_tmp_evals(RCore *core, const char *str) {
	char *s = strdup (str);
	int i, argc = r_str_split (s, ',');
	char *res = strdup ("");
	if (!s || !res) {
		free (s);
		free (res);
		return NULL;
	}
	for (i = 0; i < argc; i++) {
		char *eq, *kv = (char *)r_str_word_get0 (s, i);
		if (!kv) {
			break;
		}
		eq = strchr (kv, '=');
		if (eq) {
			*eq = 0;
			const char *ov = r_config_get (core->config, kv);
			if (!ov) {
				continue;
			}
			char *cmd = r_str_newf ("e %s=%s;", kv, ov);
			if (!cmd) {
				free (s);
				free (res);
				return NULL;
			}
			res = r_str_prepend (res, cmd);
			free (cmd);
			r_config_set (core->config, kv, eq + 1);
			*eq = '=';
		} else {
			eprintf ("Missing '=' in e: expression (%s)\n", kv);
		}
	}
	free (s);
	return res;
}