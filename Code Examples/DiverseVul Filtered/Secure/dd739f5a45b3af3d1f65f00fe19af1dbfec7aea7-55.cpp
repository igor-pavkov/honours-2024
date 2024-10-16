R_API void r_core_cmd_help(const RCore *core, const char *help[]) {
	r_cons_cmd_help (help, core->print->flags & R_PRINT_FLAGS_COLOR);
}