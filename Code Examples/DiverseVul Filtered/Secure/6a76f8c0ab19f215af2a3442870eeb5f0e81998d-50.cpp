static int ftrace_module_notify_enter(struct notifier_block *self,
				      unsigned long val, void *data)
{
	struct module *mod = data;

	if (val == MODULE_STATE_COMING)
		ftrace_init_module(mod, mod->ftrace_callsites,
				   mod->ftrace_callsites +
				   mod->num_ftrace_callsites);
	return 0;
}