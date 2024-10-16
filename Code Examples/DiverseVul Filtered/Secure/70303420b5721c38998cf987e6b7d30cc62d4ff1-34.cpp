static void update_pred_fn(struct event_filter *filter, char *fields)
{
	struct prog_entry *prog = rcu_dereference_protected(filter->prog,
						lockdep_is_held(&event_mutex));
	int i;

	for (i = 0; prog[i].pred; i++) {
		struct filter_pred *pred = prog[i].pred;
		struct ftrace_event_field *field = pred->field;

		WARN_ON_ONCE(!pred->fn);

		if (!field) {
			WARN_ONCE(1, "all leafs should have field defined %d", i);
			continue;
		}

		if (!strchr(fields, *field->name))
			continue;

		pred->fn = test_pred_visited_fn;
	}
}