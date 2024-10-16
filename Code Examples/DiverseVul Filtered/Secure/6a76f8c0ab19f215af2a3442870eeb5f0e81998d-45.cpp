static void __disable_ftrace_function_probe(void)
{
	int ret;
	int i;

	if (!ftrace_probe_registered)
		return;

	for (i = 0; i < FTRACE_FUNC_HASHSIZE; i++) {
		struct hlist_head *hhd = &ftrace_func_hash[i];
		if (hhd->first)
			return;
	}

	/* no more funcs left */
	ret = __unregister_ftrace_function(&trace_probe_ops);
	if (!ret)
		ftrace_shutdown(&trace_probe_ops, 0);

	ftrace_probe_registered = 0;
}