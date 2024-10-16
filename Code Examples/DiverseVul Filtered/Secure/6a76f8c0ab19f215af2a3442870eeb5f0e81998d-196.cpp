static void ftrace_run_update_code(int command)
{
	int ret;

	ret = ftrace_arch_code_modify_prepare();
	FTRACE_WARN_ON(ret);
	if (ret)
		return;
	/*
	 * Do not call function tracer while we update the code.
	 * We are in stop machine.
	 */
	function_trace_stop++;

	/*
	 * By default we use stop_machine() to modify the code.
	 * But archs can do what ever they want as long as it
	 * is safe. The stop_machine() is the safest, but also
	 * produces the most overhead.
	 */
	arch_ftrace_update_code(command);

	function_trace_stop--;

	ret = ftrace_arch_code_modify_post_process();
	FTRACE_WARN_ON(ret);
}