static int ftrace_module_notify_exit(struct notifier_block *self,
				     unsigned long val, void *data)
{
	struct module *mod = data;

	if (val == MODULE_STATE_GOING)
		ftrace_release_mod(mod);

	return 0;
}