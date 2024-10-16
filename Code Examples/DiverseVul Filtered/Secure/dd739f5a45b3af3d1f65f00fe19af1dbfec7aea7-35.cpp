static void cmd_autocomplete(RCore *core, const char *input) {
	RCoreAutocomplete* b = core->autocomplete;
	input = r_str_trim_ro (input);
	char arg[256];
	if (!*input) {
		print_dict (core->autocomplete, 0);
		return;
	}
	if (*input == '?') {
		r_core_cmd_help (core, help_msg_triple_exclamation);
		int i;
		r_cons_printf ("|Types:\n");
		for (i = 0; i < R_CORE_AUTOCMPLT_END; ++i) {
			if (autocomplete_flags[i].desc) {
				r_cons_printf ("| %s     %s\n",
					autocomplete_flags[i].name,
					autocomplete_flags[i].desc);
			}
		}
		return;
	}
	if (*input == '-') {
		const char *arg = input + 1;
		if (!*input) {
			eprintf ("Use !!!-* or !!!-<cmd>\n");
			return;
		}
		r_core_autocomplete_remove (b, arg);
		return;
	}
	while (b) {
		const char* end = r_str_trim_wp (input);
		if (!end) {
			break;
		}
		if ((end - input) >= sizeof (arg)) {
			// wtf?
			eprintf ("Exceeded the max arg length (255).\n");
			return;
		}
		if (end == input) {
			break;
		}
		memcpy (arg, input, end - input);
		arg[end - input] = 0;
		RCoreAutocomplete* a = r_core_autocomplete_find (b, arg, true);
		input = r_str_trim_ro (end);
		if (input && *input && !a) {
			if (b->type == R_CORE_AUTOCMPLT_DFLT && !(b = r_core_autocomplete_add (b, arg, R_CORE_AUTOCMPLT_DFLT, false))) {
				eprintf ("ENOMEM\n");
				return;
			} else if (b->type != R_CORE_AUTOCMPLT_DFLT) {
				eprintf ("Cannot add autocomplete to '%s'. type not $dflt\n", b->cmd);
				return;
			}
		} else if ((!input || !*input) && !a) {
			if (arg[0] == '$') {
				int type = autocomplete_type (arg);
				if (type != R_CORE_AUTOCMPLT_END && !b->locked && !b->n_subcmds) {
					b->type = type;
				} else if (b->locked || b->n_subcmds) {
					if (!b->cmd) {
						return;
					}
					eprintf ("Changing type of '%s' is forbidden.\n", b->cmd);
				}
			} else {
				if (!r_core_autocomplete_add (b, arg, R_CORE_AUTOCMPLT_DFLT, false)) {
					eprintf ("ENOMEM\n");
					return;
				}
			}
			return;
		} else if ((!input || !*input) && a) {
			// eprintf ("Cannot add '%s'. Already exists.\n", arg);
			return;
		} else {
			b = a;
		}
	}
	eprintf ("Invalid usage of !!!\n");
}