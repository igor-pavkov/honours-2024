static void ftrace_shutdown(struct ftrace_ops *ops, int command)
{
	bool hash_disable = true;

	if (unlikely(ftrace_disabled))
		return;

	ftrace_start_up--;
	/*
	 * Just warn in case of unbalance, no need to kill ftrace, it's not
	 * critical but the ftrace_call callers may be never nopped again after
	 * further ftrace uses.
	 */
	WARN_ON_ONCE(ftrace_start_up < 0);

	if (ops->flags & FTRACE_OPS_FL_GLOBAL) {
		ops = &global_ops;
		global_start_up--;
		WARN_ON_ONCE(global_start_up < 0);
		/* Don't update hash if global still has users */
		if (global_start_up) {
			WARN_ON_ONCE(!ftrace_start_up);
			hash_disable = false;
		}
	}

	if (hash_disable)
		ftrace_hash_rec_disable(ops, 1);

	if (ops != &global_ops || !global_start_up)
		ops->flags &= ~FTRACE_OPS_FL_ENABLED;

	command |= FTRACE_UPDATE_CALLS;

	if (saved_ftrace_func != ftrace_trace_function) {
		saved_ftrace_func = ftrace_trace_function;
		command |= FTRACE_UPDATE_TRACE_FUNC;
	}

	if (!command || !ftrace_enabled)
		return;

	ftrace_run_update_code(command);
}