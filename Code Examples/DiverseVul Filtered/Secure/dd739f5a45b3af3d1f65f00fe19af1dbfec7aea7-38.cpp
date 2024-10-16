static inline void print_dict(RCoreAutocomplete* a, int sub) {
	if (!a) {
		return;
	}
	int i, j;
	const char* name = "unknown";
	for (i = 0; i < a->n_subcmds; ++i) {
		RCoreAutocomplete* b = a->subcmds[i];
		if (b->locked) {
			continue;
		}
		for (j = 0; j < R_CORE_AUTOCMPLT_END; ++j) {
			if (b->type == autocomplete_flags[j].type) {
				name = autocomplete_flags[j].name;
				break;
			}
		}
		eprintf ("[%3d] %s: '%s'\n", sub, name, b->cmd);
		print_dict (a->subcmds[i], sub + 1);
	}
}