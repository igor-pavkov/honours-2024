static int cmd_resize(void *data, const char *input) {
	RCore *core = (RCore *)data;
	ut64 newsize = 0;
	st64 delta = 0;
	int grow, ret;

	if (cmd_r2cmd (core, input)) {
		return true;
	}

	ut64 oldsize = (core->file) ? r_io_fd_size (core->io, core->file->fd): 0;
	switch (*input) {
	case 'a': // "r..."
		if (r_str_startswith (input, "adare2")) {
			__runMain (core->r_main_radare2, input - 1); 
		}
		return true;
	case '2': // "r2" // XXX should be handled already in cmd_r2cmd()
		// TODO: use argv[0] instead of 'radare2'
		r_sys_cmdf ("radare%s", input);
		return true;
	case 'm': // "rm"
		if (input[1] == ' ') {
			const char *file = r_str_trim_ro (input + 2);
			if (*file == '$') {
				r_cmd_alias_del (core->rcmd, file);
			} else {
				r_file_rm (file);
			}
		} else {
			eprintf ("Usage: rm [file]   # removes a file\n");
		}
		return true;
	case '\0':
		if (core->file) {
			if (oldsize != -1) {
				r_cons_printf ("%"PFMT64d"\n", oldsize);
			}
		}
		return true;
	case 'h':
		if (core->file) {
			if (oldsize != -1) {
				char humansz[8];
				r_num_units (humansz, sizeof (humansz), oldsize);
				r_cons_printf ("%s\n", humansz);
			}
		}
		return true;
	case '+': // "r+"
	case '-': // "r-"
		delta = (st64)r_num_math (core->num, input);
		newsize = oldsize + delta;
		break;
	case ' ': // "r "
		newsize = r_num_math (core->num, input + 1);
		if (newsize == 0) {
			if (input[1] == '0') {
				eprintf ("Invalid size\n");
			}
			return false;
		}
		break;
	case 'e':
		write (1, Color_RESET_TERMINAL, strlen (Color_RESET_TERMINAL));
		return true;
	case '?': // "r?"
	default:
		r_core_cmd_help (core, help_msg_r);
		return true;
	}

	grow = (newsize > oldsize);
	if (grow) {
		ret = r_io_resize (core->io, newsize);
		if (ret < 1) {
			eprintf ("r_io_resize: cannot resize\n");
		}
	}
	if (delta && core->offset < newsize) {
		r_io_shift (core->io, core->offset, grow?newsize:oldsize, delta);
	}
	if (!grow) {
		ret = r_io_resize (core->io, newsize);
		if (ret < 1) {
			eprintf ("r_io_resize: cannot resize\n");
		}
	}
	if (newsize < core->offset+core->blocksize || oldsize < core->offset + core->blocksize) {
		r_core_block_read (core);
	}
	return true;
}