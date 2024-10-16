static int ftrace_function_set_filter(struct perf_event *event,
				      struct event_filter *filter)
{
	struct prog_entry *prog = rcu_dereference_protected(filter->prog,
						lockdep_is_held(&event_mutex));
	struct function_filter_data data = {
		.first_filter  = 1,
		.first_notrace = 1,
		.ops           = &event->ftrace_ops,
	};
	int i;

	for (i = 0; prog[i].pred; i++) {
		struct filter_pred *pred = prog[i].pred;

		if (!is_or(prog, i))
			return -EINVAL;

		if (ftrace_function_set_filter_pred(pred, &data) < 0)
			return -EINVAL;
	}
	return 0;
}