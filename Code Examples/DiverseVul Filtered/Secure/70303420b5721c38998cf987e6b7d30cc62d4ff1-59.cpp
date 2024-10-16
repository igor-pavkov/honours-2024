static int process_preds(struct trace_event_call *call,
			 const char *filter_string,
			 struct event_filter *filter,
			 struct filter_parse_error *pe)
{
	struct prog_entry *prog;
	int nr_parens;
	int nr_preds;
	int index;
	int ret;

	ret = calc_stack(filter_string, &nr_parens, &nr_preds, &index);
	if (ret < 0) {
		switch (ret) {
		case MISSING_QUOTE:
			parse_error(pe, FILT_ERR_MISSING_QUOTE, index);
			break;
		case TOO_MANY_OPEN:
			parse_error(pe, FILT_ERR_TOO_MANY_OPEN, index);
			break;
		default:
			parse_error(pe, FILT_ERR_TOO_MANY_CLOSE, index);
		}
		return ret;
	}

	if (!nr_preds)
		return -EINVAL;

	prog = predicate_parse(filter_string, nr_parens, nr_preds,
			       parse_pred, call, pe);
	if (IS_ERR(prog))
		return PTR_ERR(prog);

	rcu_assign_pointer(filter->prog, prog);
	return 0;
}