int ptrace_getregs(struct task_struct *child, void __user *uregs)
{
	struct pt_regs *regs = task_pt_regs(child);
	xtensa_gregset_t __user *gregset = uregs;
	unsigned long wm = regs->wmask;
	unsigned long wb = regs->windowbase;
	int live, i;

	if (!access_ok(VERIFY_WRITE, uregs, sizeof(xtensa_gregset_t)))
		return -EIO;

	__put_user(regs->pc, &gregset->pc);
	__put_user(regs->ps & ~(1 << PS_EXCM_BIT), &gregset->ps);
	__put_user(regs->lbeg, &gregset->lbeg);
	__put_user(regs->lend, &gregset->lend);
	__put_user(regs->lcount, &gregset->lcount);
	__put_user(regs->windowstart, &gregset->windowstart);
	__put_user(regs->windowbase, &gregset->windowbase);

	live = (wm & 2) ? 4 : (wm & 4) ? 8 : (wm & 8) ? 12 : 16;

	for (i = 0; i < live; i++)
		__put_user(regs->areg[i],gregset->a+((wb*4+i)%XCHAL_NUM_AREGS));
	for (i = XCHAL_NUM_AREGS - (wm >> 4) * 4; i < XCHAL_NUM_AREGS; i++)
		__put_user(regs->areg[i],gregset->a+((wb*4+i)%XCHAL_NUM_AREGS));

	return 0;
}