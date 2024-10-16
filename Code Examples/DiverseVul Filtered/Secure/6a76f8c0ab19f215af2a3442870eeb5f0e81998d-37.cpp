static void ftrace_pid_func(unsigned long ip, unsigned long parent_ip,
			    struct ftrace_ops *op, struct pt_regs *regs)
{
	if (!test_tsk_trace_trace(current))
		return;

	ftrace_pid_function(ip, parent_ip, op, regs);
}