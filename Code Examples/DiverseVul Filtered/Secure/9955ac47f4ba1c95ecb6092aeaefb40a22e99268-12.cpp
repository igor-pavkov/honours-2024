static void dump_backtrace_entry(unsigned long where, unsigned long stack)
{
	print_ip_sym(where);
	if (in_exception_text(where))
		dump_mem("", "Exception stack", stack,
			 stack + sizeof(struct pt_regs));
}