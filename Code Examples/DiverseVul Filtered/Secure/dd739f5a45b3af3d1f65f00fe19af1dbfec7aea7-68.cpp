R_API void run_pending_anal(RCore *core) {
	// allow incall events in the run_pending step
	core->ev->incall = false;
	if (core && core->anal && core->anal->cmdtail) {
		char *res = core->anal->cmdtail;
		core->anal->cmdtail = NULL;
		r_core_cmd_lines (core, res);
		free (res);
	}
}