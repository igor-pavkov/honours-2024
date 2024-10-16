static int cmd_bsize(void *data, const char *input) {
	ut64 n;
	RFlagItem *flag;
	RCore *core = (RCore *)data;
	switch (input[0]) {
	case 'm': // "bm"
		n = r_num_math (core->num, input + 1);
		if (n > 1) {
			core->blocksize_max = n;
		} else {
			r_cons_printf ("0x%x\n", (ut32)core->blocksize_max);
		}
		break;
	case '+': // "b+"
		n = r_num_math (core->num, input + 1);
		r_core_block_size (core, core->blocksize + n);
		break;
	case '-': // "b-"
		n = r_num_math (core->num, input + 1);
		r_core_block_size (core, core->blocksize - n);
		break;
	case 'f': // "bf"
		if (input[1] == ' ') {
			flag = r_flag_get (core->flags, input + 2);
			if (flag) {
				r_core_block_size (core, flag->size);
			} else {
				eprintf ("bf: cannot find flag named '%s'\n", input + 2);
			}
		} else {
			eprintf ("Usage: bf [flagname]\n");
		}
		break;
	case 'j': // "bj"
		r_cons_printf ("{\"blocksize\":%d,\"blocksize_limit\":%d}\n", core->blocksize, core->blocksize_max);
		break;
	case '*': // "b*"
		r_cons_printf ("b 0x%x\n", core->blocksize);
		break;
	case '\0': // "b"
		r_cons_printf ("0x%x\n", core->blocksize);
		break;
	case ' ':
		r_core_block_size (core, r_num_math (core->num, input));
		break;
	default:
	case '?': // "b?"
		r_core_cmd_help (core, help_msg_b);
		break;
	}
	return 0;
}