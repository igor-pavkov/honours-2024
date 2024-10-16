static int __ftrace_function_set_filter(int filter, char *buf, int len,
					struct function_filter_data *data)
{
	int i, re_cnt, ret = -EINVAL;
	int *reset;
	char **re;

	reset = filter ? &data->first_filter : &data->first_notrace;

	/*
	 * The 'ip' field could have multiple filters set, separated
	 * either by space or comma. We first cut the filter and apply
	 * all pieces separatelly.
	 */
	re = ftrace_function_filter_re(buf, len, &re_cnt);
	if (!re)
		return -EINVAL;

	for (i = 0; i < re_cnt; i++) {
		ret = ftrace_function_set_regexp(data->ops, filter, *reset,
						 re[i], strlen(re[i]));
		if (ret)
			break;

		if (*reset)
			*reset = 0;
	}

	argv_free(re);
	return ret;
}