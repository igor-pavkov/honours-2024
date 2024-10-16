static void ds_update_cea(void *cea, void *addr, size_t size, pgprot_t prot)
{
	unsigned long start = (unsigned long)cea;
	phys_addr_t pa;
	size_t msz = 0;

	pa = virt_to_phys(addr);

	preempt_disable();
	for (; msz < size; msz += PAGE_SIZE, pa += PAGE_SIZE, cea += PAGE_SIZE)
		cea_set_pte(cea, pa, prot);

	/*
	 * This is a cross-CPU update of the cpu_entry_area, we must shoot down
	 * all TLB entries for it.
	 */
	flush_tlb_kernel_range(start, start + size);
	preempt_enable();
}