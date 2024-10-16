int ptrace_peekusr(struct task_struct *child, long regno, long __user *ret)
{
	struct pt_regs *regs;
	unsigned long tmp;

	regs = task_pt_regs(child);
	tmp = 0;  /* Default return value. */

	switch(regno) {

		case REG_AR_BASE ... REG_AR_BASE + XCHAL_NUM_AREGS - 1:
			tmp = regs->areg[regno - REG_AR_BASE];
			break;

		case REG_A_BASE ... REG_A_BASE + 15:
			tmp = regs->areg[regno - REG_A_BASE];
			break;

		case REG_PC:
			tmp = regs->pc;
			break;

		case REG_PS:
			/* Note:  PS.EXCM is not set while user task is running;
			 * its being set in regs is for exception handling
			 * convenience.  */
			tmp = (regs->ps & ~(1 << PS_EXCM_BIT));
			break;

		case REG_WB:
			break;		/* tmp = 0 */

		case REG_WS:
		{
			unsigned long wb = regs->windowbase;
			unsigned long ws = regs->windowstart;
			tmp = ((ws>>wb) | (ws<<(WSBITS-wb))) & ((1<<WSBITS)-1);
			break;
		}
		case REG_LBEG:
			tmp = regs->lbeg;
			break;

		case REG_LEND:
			tmp = regs->lend;
			break;

		case REG_LCOUNT:
			tmp = regs->lcount;
			break;

		case REG_SAR:
			tmp = regs->sar;
			break;

		case SYSCALL_NR:
			tmp = regs->syscall;
			break;

		default:
			return -EIO;
	}
	return put_user(tmp, ret);
}