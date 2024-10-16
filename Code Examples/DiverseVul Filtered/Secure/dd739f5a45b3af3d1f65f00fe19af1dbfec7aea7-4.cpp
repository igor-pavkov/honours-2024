static int r_core_cmd_nullcallback(void *data) {
	RCore *core = (RCore*) data;
	if (core->cons->context->breaked) {
		core->cons->context->breaked = false;
		return 0;
	}
	if (!core->cmdrepeat) {
		return 0;
	}
	r_core_cmd_repeat (core, true);
	return 1;
}