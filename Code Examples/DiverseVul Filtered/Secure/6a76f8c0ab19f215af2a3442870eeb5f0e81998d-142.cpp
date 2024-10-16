void ftrace_run_stop_machine(int command)
{
	stop_machine(__ftrace_modify_code, &command, NULL);
}