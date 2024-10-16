static void update_global_ops(void)
{
	ftrace_func_t func;

	/*
	 * If there's only one function registered, then call that
	 * function directly. Otherwise, we need to iterate over the
	 * registered callers.
	 */
	if (ftrace_global_list == &ftrace_list_end ||
	    ftrace_global_list->next == &ftrace_list_end) {
		func = ftrace_global_list->func;
		/*
		 * As we are calling the function directly.
		 * If it does not have recursion protection,
		 * the function_trace_op needs to be updated
		 * accordingly.
		 */
		if (ftrace_global_list->flags & FTRACE_OPS_FL_RECURSION_SAFE)
			global_ops.flags |= FTRACE_OPS_FL_RECURSION_SAFE;
		else
			global_ops.flags &= ~FTRACE_OPS_FL_RECURSION_SAFE;
	} else {
		func = ftrace_global_list_func;
		/* The list has its own recursion protection. */
		global_ops.flags |= FTRACE_OPS_FL_RECURSION_SAFE;
	}


	/* If we filter on pids, update to use the pid function */
	if (!list_empty(&ftrace_pids)) {
		set_ftrace_pid_function(func);
		func = ftrace_pid_func;
	}

	global_ops.func = func;
}