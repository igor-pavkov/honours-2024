int set_thread_uses_vas(void)
{
#ifdef CONFIG_PPC_BOOK3S_64
	if (!cpu_has_feature(CPU_FTR_ARCH_300))
		return -EINVAL;

	current->thread.used_vas = 1;

	/*
	 * Even a process that has no foreign real address mapping can use
	 * an unpaired COPY instruction (to no real effect). Issue CP_ABORT
	 * to clear any pending COPY and prevent a covert channel.
	 *
	 * __switch_to() will issue CP_ABORT on future context switches.
	 */
	asm volatile(PPC_CP_ABORT);

#endif /* CONFIG_PPC_BOOK3S_64 */
	return 0;
}