static void recursive_help_go(RCore *core, int detail, RCmdDescriptor *desc) {
	int i;
	if (desc->help_msg) {
		r_core_cmd_help (core, desc->help_msg);
	}
	if (detail >= 1) {
		if (desc->help_detail) {
			r_core_cmd_help (core, desc->help_detail);
		}
		if (detail >= 2 && desc->help_detail2) {
			r_core_cmd_help (core, desc->help_detail2);
		}
	}
	for (i = 32; i < R_ARRAY_SIZE (desc->sub); i++) {
		if (desc->sub[i]) {
			recursive_help_go (core, detail, desc->sub[i]);
		}
	}
}