void notrace __ppc64_runlatch_on(void)
{
	struct thread_info *ti = current_thread_info();

	if (cpu_has_feature(CPU_FTR_ARCH_206)) {
		/*
		 * Least significant bit (RUN) is the only writable bit of
		 * the CTRL register, so we can avoid mfspr. 2.06 is not the
		 * earliest ISA where this is the case, but it's convenient.
		 */
		mtspr(SPRN_CTRLT, CTRL_RUNLATCH);
	} else {
		unsigned long ctrl;

		/*
		 * Some architectures (e.g., Cell) have writable fields other
		 * than RUN, so do the read-modify-write.
		 */
		ctrl = mfspr(SPRN_CTRLF);
		ctrl |= CTRL_RUNLATCH;
		mtspr(SPRN_CTRLT, ctrl);
	}

	ti->local_flags |= _TLF_RUNLATCH;
}