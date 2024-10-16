static inline unsigned long __get_current_cr3_fast(void)
{
	unsigned long cr3 = __pa(this_cpu_read(cpu_tlbstate.loaded_mm)->pgd);

	/* For now, be very restrictive about when this can be called. */
	VM_WARN_ON(in_nmi() || preemptible());

	VM_BUG_ON(cr3 != __read_cr3());
	return cr3;
}