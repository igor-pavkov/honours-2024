int set_fpexc_mode(struct task_struct *tsk, unsigned int val)
{
	struct pt_regs *regs = tsk->thread.regs;

	/* This is a bit hairy.  If we are an SPE enabled  processor
	 * (have embedded fp) we store the IEEE exception enable flags in
	 * fpexc_mode.  fpexc_mode is also used for setting FP exception
	 * mode (asyn, precise, disabled) for 'Classic' FP. */
	if (val & PR_FP_EXC_SW_ENABLE) {
#ifdef CONFIG_SPE
		if (cpu_has_feature(CPU_FTR_SPE)) {
			/*
			 * When the sticky exception bits are set
			 * directly by userspace, it must call prctl
			 * with PR_GET_FPEXC (with PR_FP_EXC_SW_ENABLE
			 * in the existing prctl settings) or
			 * PR_SET_FPEXC (with PR_FP_EXC_SW_ENABLE in
			 * the bits being set).  <fenv.h> functions
			 * saving and restoring the whole
			 * floating-point environment need to do so
			 * anyway to restore the prctl settings from
			 * the saved environment.
			 */
			tsk->thread.spefscr_last = mfspr(SPRN_SPEFSCR);
			tsk->thread.fpexc_mode = val &
				(PR_FP_EXC_SW_ENABLE | PR_FP_ALL_EXCEPT);
			return 0;
		} else {
			return -EINVAL;
		}
#else
		return -EINVAL;
#endif
	}

	/* on a CONFIG_SPE this does not hurt us.  The bits that
	 * __pack_fe01 use do not overlap with bits used for
	 * PR_FP_EXC_SW_ENABLE.  Additionally, the MSR[FE0,FE1] bits
	 * on CONFIG_SPE implementations are reserved so writing to
	 * them does not change anything */
	if (val > PR_FP_EXC_PRECISE)
		return -EINVAL;
	tsk->thread.fpexc_mode = __pack_fe01(val);
	if (regs != NULL && (regs->msr & MSR_FP) != 0)
		regs->msr = (regs->msr & ~(MSR_FE0|MSR_FE1))
			| tsk->thread.fpexc_mode;
	return 0;
}