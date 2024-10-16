void __weak arch_ftrace_update_code(int command)
{
	ftrace_run_stop_machine(command);
}