void notrace __ppc64_runlatch_off(void)
{
	struct thread_info *ti = current_thread_info();

	ti->local_flags &= ~_TLF_RUNLATCH;

	if (cpu_has_feature(CPU_FTR_ARCH_206)) {
		mtspr(SPRN_CTRLT, 0);
	} else {
		unsigned long ctrl;

		ctrl = mfspr(SPRN_CTRLF);
		ctrl &= ~CTRL_RUNLATCH;
		mtspr(SPRN_CTRLT, ctrl);
	}
}