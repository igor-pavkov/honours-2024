static void profile_graph_return(struct ftrace_graph_ret *trace)
{
	struct ftrace_profile_stat *stat;
	unsigned long long calltime;
	struct ftrace_profile *rec;
	unsigned long flags;

	local_irq_save(flags);
	stat = &__get_cpu_var(ftrace_profile_stats);
	if (!stat->hash || !ftrace_profile_enabled)
		goto out;

	/* If the calltime was zero'd ignore it */
	if (!trace->calltime)
		goto out;

	calltime = trace->rettime - trace->calltime;

	if (!(trace_flags & TRACE_ITER_GRAPH_TIME)) {
		int index;

		index = trace->depth;

		/* Append this call time to the parent time to subtract */
		if (index)
			current->ret_stack[index - 1].subtime += calltime;

		if (current->ret_stack[index].subtime < calltime)
			calltime -= current->ret_stack[index].subtime;
		else
			calltime = 0;
	}

	rec = ftrace_find_profiled_func(stat, trace->func);
	if (rec) {
		rec->time += calltime;
		rec->time_squared += calltime * calltime;
	}

 out:
	local_irq_restore(flags);
}