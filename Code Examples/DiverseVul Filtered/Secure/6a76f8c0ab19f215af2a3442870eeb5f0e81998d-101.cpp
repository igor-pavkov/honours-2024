int register_ftrace_graph(trace_func_graph_ret_t retfunc,
			trace_func_graph_ent_t entryfunc)
{
	int ret = 0;

	mutex_lock(&ftrace_lock);

	/* we currently allow only one tracer registered at a time */
	if (ftrace_graph_active) {
		ret = -EBUSY;
		goto out;
	}

	ftrace_suspend_notifier.notifier_call = ftrace_suspend_notifier_call;
	register_pm_notifier(&ftrace_suspend_notifier);

	ftrace_graph_active++;
	ret = start_graph_tracing();
	if (ret) {
		ftrace_graph_active--;
		goto out;
	}

	ftrace_graph_return = retfunc;
	ftrace_graph_entry = entryfunc;

	ret = ftrace_startup(&global_ops, FTRACE_START_FUNC_RET);

out:
	mutex_unlock(&ftrace_lock);
	return ret;
}