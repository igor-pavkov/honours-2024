static int create_filter(struct trace_event_call *call,
			 char *filter_string, bool set_str,
			 struct event_filter **filterp)
{
	struct filter_parse_error *pe = NULL;
	int err;

	err = create_filter_start(filter_string, set_str, &pe, filterp);
	if (err)
		return err;

	err = process_preds(call, filter_string, *filterp, pe);
	if (err && set_str)
		append_filter_err(pe, *filterp);

	return err;
}