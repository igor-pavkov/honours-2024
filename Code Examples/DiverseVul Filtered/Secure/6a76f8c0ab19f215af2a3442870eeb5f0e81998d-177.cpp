void __init ftrace_init(void)
{
	unsigned long count, addr, flags;
	int ret;

	/* Keep the ftrace pointer to the stub */
	addr = (unsigned long)ftrace_stub;

	local_irq_save(flags);
	ftrace_dyn_arch_init(&addr);
	local_irq_restore(flags);

	/* ftrace_dyn_arch_init places the return code in addr */
	if (addr)
		goto failed;

	count = __stop_mcount_loc - __start_mcount_loc;

	ret = ftrace_dyn_table_alloc(count);
	if (ret)
		goto failed;

	last_ftrace_enabled = ftrace_enabled = 1;

	ret = ftrace_process_locs(NULL,
				  __start_mcount_loc,
				  __stop_mcount_loc);

	ret = register_module_notifier(&ftrace_module_enter_nb);
	if (ret)
		pr_warning("Failed to register trace ftrace module enter notifier\n");

	ret = register_module_notifier(&ftrace_module_exit_nb);
	if (ret)
		pr_warning("Failed to register trace ftrace module exit notifier\n");

	set_ftrace_early_filters();

	return;
 failed:
	ftrace_disabled = 1;
}