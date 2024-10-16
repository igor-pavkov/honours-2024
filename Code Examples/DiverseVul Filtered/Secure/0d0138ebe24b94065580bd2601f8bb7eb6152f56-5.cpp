int ptrace_getxregs(struct task_struct *child, void __user *uregs)
{
	struct pt_regs *regs = task_pt_regs(child);
	struct thread_info *ti = task_thread_info(child);
	elf_xtregs_t __user *xtregs = uregs;
	int ret = 0;

	if (!access_ok(VERIFY_WRITE, uregs, sizeof(elf_xtregs_t)))
		return -EIO;

#if XTENSA_HAVE_COPROCESSORS
	/* Flush all coprocessor registers to memory. */
	coprocessor_flush_all(ti);
	ret |= __copy_to_user(&xtregs->cp0, &ti->xtregs_cp,
			      sizeof(xtregs_coprocessor_t));
#endif
	ret |= __copy_to_user(&xtregs->opt, &regs->xtregs_opt,
			      sizeof(xtregs->opt));
	ret |= __copy_to_user(&xtregs->user,&ti->xtregs_user,
			      sizeof(xtregs->user));

	return ret ? -EFAULT : 0;
}