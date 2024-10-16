static void foreach_pairs(RCore *core, const char *cmd, const char *each) {
	const char *arg;
	int pair = 0;
	for (arg = each ; ; ) {
		char *next = strchr (arg, ' ');
		if (next) {
			*next = 0;
		}
		if (arg && *arg) {
			ut64 n = r_num_get (NULL, arg);
			if (pair%2) {
				r_core_block_size (core, n);
				r_core_cmd0 (core, cmd);
			} else {
				r_core_seek (core, n, 1);
			}
			pair++;
		}
		if (!next) {
			break;
		}
		arg = next + 1;
	}
}