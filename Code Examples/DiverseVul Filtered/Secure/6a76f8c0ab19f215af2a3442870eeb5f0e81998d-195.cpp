static void ftrace_init_module(struct module *mod,
			       unsigned long *start, unsigned long *end)
{
	if (ftrace_disabled || start == end)
		return;
	ftrace_process_locs(mod, start, end);
}