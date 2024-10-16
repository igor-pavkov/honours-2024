static void amd_k7_smp_check(struct cpuinfo_x86 *c)
{
	/* calling is from identify_secondary_cpu() ? */
	if (!c->cpu_index)
		return;

	/*
	 * Certain Athlons might work (for various values of 'work') in SMP
	 * but they are not certified as MP capable.
	 */
	/* Athlon 660/661 is valid. */
	if ((c->x86_model == 6) && ((c->x86_mask == 0) ||
	    (c->x86_mask == 1)))
		return;

	/* Duron 670 is valid */
	if ((c->x86_model == 7) && (c->x86_mask == 0))
		return;

	/*
	 * Athlon 662, Duron 671, and Athlon >model 7 have capability
	 * bit. It's worth noting that the A5 stepping (662) of some
	 * Athlon XP's have the MP bit set.
	 * See http://www.heise.de/newsticker/data/jow-18.10.01-000 for
	 * more.
	 */
	if (((c->x86_model == 6) && (c->x86_mask >= 2)) ||
	    ((c->x86_model == 7) && (c->x86_mask >= 1)) ||
	     (c->x86_model > 7))
		if (cpu_has_mp)
			return;

	/* If we get here, not a certified SMP capable AMD system. */

	/*
	 * Don't taint if we are running SMP kernel on a single non-MP
	 * approved Athlon
	 */
	WARN_ONCE(1, "WARNING: This combination of AMD"
		" processors is not suitable for SMP.\n");
	add_taint(TAINT_UNSAFE_SMP, LOCKDEP_NOW_UNRELIABLE);
}