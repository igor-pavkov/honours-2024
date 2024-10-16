R_API int r_core_cmd_task_sync(RCore *core, const char *cmd, bool log) {
	RCoreTask *task = core->main_task;
	char *s = strdup (cmd);
	if (!s) {
		return 0;
	}
	task->cmd = s;
	task->cmd_log = log;
	task->state = R_CORE_TASK_STATE_BEFORE_START;
	int res = r_core_task_run_sync (core, task);
	free (s);
	return res;
}