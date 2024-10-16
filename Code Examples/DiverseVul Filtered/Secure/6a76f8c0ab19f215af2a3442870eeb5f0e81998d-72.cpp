static void ftrace_ops_list_func(unsigned long ip, unsigned long parent_ip,
				 struct ftrace_ops *op, struct pt_regs *regs)
{
	__ftrace_ops_list_func(ip, parent_ip, NULL, regs);
}