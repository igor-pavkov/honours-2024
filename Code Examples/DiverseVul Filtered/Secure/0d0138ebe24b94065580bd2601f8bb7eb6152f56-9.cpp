void do_syscall_trace_enter(struct pt_regs *regs)
{
	if (test_thread_flag(TIF_SYSCALL_TRACE)
			&& (current->ptrace & PT_PTRACED))
		do_syscall_trace();

#if 0
	if (unlikely(current->audit_context))
		audit_syscall_entry(current, AUDIT_ARCH_XTENSA..);
#endif
}