static inline void switch_ldt(struct mm_struct *prev, struct mm_struct *next)
{
#ifdef CONFIG_MODIFY_LDT_SYSCALL
	/*
	 * Load the LDT if either the old or new mm had an LDT.
	 *
	 * An mm will never go from having an LDT to not having an LDT.  Two
	 * mms never share an LDT, so we don't gain anything by checking to
	 * see whether the LDT changed.  There's also no guarantee that
	 * prev->context.ldt actually matches LDTR, but, if LDTR is non-NULL,
	 * then prev->context.ldt will also be non-NULL.
	 *
	 * If we really cared, we could optimize the case where prev == next
	 * and we're exiting lazy mode.  Most of the time, if this happens,
	 * we don't actually need to reload LDTR, but modify_ldt() is mostly
	 * used by legacy code and emulators where we don't need this level of
	 * performance.
	 *
	 * This uses | instead of || because it generates better code.
	 */
	if (unlikely((unsigned long)prev->context.ldt |
		     (unsigned long)next->context.ldt))
		load_mm_ldt(next);
#endif

	DEBUG_LOCKS_WARN_ON(preemptible());
}