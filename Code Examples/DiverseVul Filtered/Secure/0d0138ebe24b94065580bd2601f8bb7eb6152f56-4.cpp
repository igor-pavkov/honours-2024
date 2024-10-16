int ptrace_pokeusr(struct task_struct *child, long regno, long val)
{
	struct pt_regs *regs;
	regs = task_pt_regs(child);

	switch (regno) {
		case REG_AR_BASE ... REG_AR_BASE + XCHAL_NUM_AREGS - 1:
			regs->areg[regno - REG_AR_BASE] = val;
			break;

		case REG_A_BASE ... REG_A_BASE + 15:
			regs->areg[regno - REG_A_BASE] = val;
			break;

		case REG_PC:
			regs->pc = val;
			break;

		case SYSCALL_NR:
			regs->syscall = val;
			break;

		default:
			return -EIO;
	}
	return 0;
}