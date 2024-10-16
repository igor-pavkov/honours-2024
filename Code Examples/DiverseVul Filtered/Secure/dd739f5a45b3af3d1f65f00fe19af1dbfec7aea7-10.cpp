R_API void r_core_cmd_repeat(RCore *core, int next) {
	// Fix for backtickbug px`~`
	if (!core->lastcmd || core->cons->context->cmd_depth < 1) {
		return;
	}
	switch (*core->lastcmd) {
	case '.':
		if (core->lastcmd[1] == '(') { // macro call
			r_core_cmd0 (core, core->lastcmd);
		}
		break;
	case 'd': // debug
		r_core_cmd0 (core, core->lastcmd);
		switch (core->lastcmd[1]) {
		case 's':
		case 'c':
			r_core_cmd0 (core, "sr PC;pd 1");
		}
		break;
	case 'p': // print
	case 'x':
	case '$':
		if (!strncmp (core->lastcmd, "pd", 2)) {
			if (core->lastcmd[2]== ' ') {
				r_core_cmdf (core, "so %s", core->lastcmd + 3);
			} else {
				r_core_cmd0 (core, "so `pi~?`");
			}
		} else {
			if (next) {
				r_core_seek (core, core->offset + core->blocksize, 1);
			} else {
				if (core->blocksize > core->offset) {
					r_core_seek (core, 0, 1);
				} else {
					r_core_seek (core, core->offset - core->blocksize, 1);
				}
			}
		}
		r_core_cmd0 (core, core->lastcmd);
		break;
	}
}