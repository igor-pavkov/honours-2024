void do_send_trap(struct pt_regs *regs, unsigned long address,
		  unsigned long error_code, int breakpt)
{
	current->thread.trap_nr = TRAP_HWBKPT;
	if (notify_die(DIE_DABR_MATCH, "dabr_match", regs, error_code,
			11, SIGSEGV) == NOTIFY_STOP)
		return;

	/* Deliver the signal to userspace */
	force_sig_ptrace_errno_trap(breakpt, /* breakpoint or watchpoint id */
				    (void __user *)address);
}