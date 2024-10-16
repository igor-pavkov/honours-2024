static int core_cmd0_wrapper(void *core, const char *cmd) {
	return r_core_cmd0 ((RCore *)core, cmd);
}