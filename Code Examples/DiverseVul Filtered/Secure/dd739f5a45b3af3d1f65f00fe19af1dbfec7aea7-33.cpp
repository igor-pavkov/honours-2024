static int autocomplete_type(const char* strflag) {
	int i;
	for (i = 0; i < R_CORE_AUTOCMPLT_END; ++i) {
		if (autocomplete_flags[i].desc && !strncmp (strflag, autocomplete_flags[i].name, 5)) {
			return autocomplete_flags[i].type;
		}
	}
	eprintf ("Invalid flag '%s'\n", strflag);
	return R_CORE_AUTOCMPLT_END;
}