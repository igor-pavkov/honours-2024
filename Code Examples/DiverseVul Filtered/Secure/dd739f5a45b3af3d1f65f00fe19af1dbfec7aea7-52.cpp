static void cmd_descriptor_init(RCore *core) {
	const ut8 *p;
	RListIter *iter;
	RCmdDescriptor *x, *y;
	int n = core->cmd_descriptors->length;
	r_list_sort (core->cmd_descriptors, compare_cmd_descriptor_name);
	r_list_foreach (core->cmd_descriptors, iter, y) {
		if (--n < 0) {
			break;
		}
		x = &core->root_cmd_descriptor;
		for (p = (const ut8 *)y->cmd; *p; p++) {
			if (!x->sub[*p]) {
				if (p[1]) {
					RCmdDescriptor *d = R_NEW0 (RCmdDescriptor);
					r_list_append (core->cmd_descriptors, d);
					x->sub[*p] = d;
				} else {
					x->sub[*p] = y;
				}
			} else if (!p[1]) {
				eprintf ("Command '%s' is duplicated, please check\n", y->cmd);
			}
			x = x->sub[*p];
		}
	}
}