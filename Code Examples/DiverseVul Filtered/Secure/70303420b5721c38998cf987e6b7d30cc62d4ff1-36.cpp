static int create_filter_start(char *filter_string, bool set_str,
			       struct filter_parse_error **pse,
			       struct event_filter **filterp)
{
	struct event_filter *filter;
	struct filter_parse_error *pe = NULL;
	int err = 0;

	if (WARN_ON_ONCE(*pse || *filterp))
		return -EINVAL;

	filter = kzalloc(sizeof(*filter), GFP_KERNEL);
	if (filter && set_str) {
		filter->filter_string = kstrdup(filter_string, GFP_KERNEL);
		if (!filter->filter_string)
			err = -ENOMEM;
	}

	pe = kzalloc(sizeof(*pe), GFP_KERNEL);

	if (!filter || !pe || err) {
		kfree(pe);
		__free_filter(filter);
		return -ENOMEM;
	}

	/* we're committed to creating a new filter */
	*filterp = filter;
	*pse = pe;

	return 0;
}