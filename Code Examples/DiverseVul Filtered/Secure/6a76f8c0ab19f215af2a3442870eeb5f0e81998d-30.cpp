static void __init set_ftrace_early_graph(char *buf)
{
	int ret;
	char *func;

	while (buf) {
		func = strsep(&buf, ",");
		/* we allow only one expression at a time */
		ret = ftrace_set_func(ftrace_graph_funcs, &ftrace_graph_count,
				      func);
		if (ret)
			printk(KERN_DEBUG "ftrace: function %s not "
					  "traceable\n", func);
	}
}