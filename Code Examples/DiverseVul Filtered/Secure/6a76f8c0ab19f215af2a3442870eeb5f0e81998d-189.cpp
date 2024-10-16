static void __enable_ftrace_function_probe(void)
{
	int ret;
	int i;

	if (ftrace_probe_registered)
		return;

	for (i = 0; i < FTRACE_FUNC_HASHSIZE; i++) {
		struct hlist_head *hhd = &ftrace_func_hash[i];
		if (hhd->first)
			break;
	}
	/* Nothing registered? */
	if (i == FTRACE_FUNC_HASHSIZE)
		return;

	ret = __register_ftrace_function(&trace_probe_ops);
	if (!ret)
		ret = ftrace_startup(&trace_probe_ops, 0);

	ftrace_probe_registered = 1;
}