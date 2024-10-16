R_API int r_core_cmd_lines(RCore *core, const char *lines) {
	int r, ret = true;
	char *nl, *data, *odata;

	if (!lines || !*lines) {
		return true;
	}
	data = odata = strdup (lines);
	if (!odata) {
		return false;
	}
	nl = strchr (odata, '\n');
	if (nl) {
		r_cons_break_push (NULL, NULL);
		do {
			if (r_cons_is_breaked ()) {
				free (odata);
				r_cons_break_pop ();
				return ret;
			}
			*nl = '\0';
			r = r_core_cmd (core, data, 0);
			if (r < 0) { //== -1) {
				data = nl + 1;
				ret = -1; //r; //false;
				break;
			}
			r_cons_flush ();
			if (data[0] == 'q') {
				if (data[1] == '!') {
					ret = -1;
				} else {
					eprintf ("'q': quit ignored. Use 'q!'\n");
				}
				data = nl + 1;
				break;
			}
			data = nl + 1;
		} while ((nl = strchr (data, '\n')));
		r_cons_break_pop ();
	}
	if (ret >= 0 && data && *data) {
		r_core_cmd (core, data, 0);
	}
	free (odata);
	return ret;
}