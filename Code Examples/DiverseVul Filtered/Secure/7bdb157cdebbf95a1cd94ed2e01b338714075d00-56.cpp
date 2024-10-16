static u64 perf_virt_to_phys(u64 virt)
{
	u64 phys_addr = 0;
	struct page *p = NULL;

	if (!virt)
		return 0;

	if (virt >= TASK_SIZE) {
		/* If it's vmalloc()d memory, leave phys_addr as 0 */
		if (virt_addr_valid((void *)(uintptr_t)virt) &&
		    !(virt >= VMALLOC_START && virt < VMALLOC_END))
			phys_addr = (u64)virt_to_phys((void *)(uintptr_t)virt);
	} else {
		/*
		 * Walking the pages tables for user address.
		 * Interrupts are disabled, so it prevents any tear down
		 * of the page tables.
		 * Try IRQ-safe get_user_page_fast_only first.
		 * If failed, leave phys_addr as 0.
		 */
		if (current->mm != NULL) {
			pagefault_disable();
			if (get_user_page_fast_only(virt, 0, &p))
				phys_addr = page_to_phys(p) + virt % PAGE_SIZE;
			pagefault_enable();
		}

		if (p)
			put_page(p);
	}

	return phys_addr;
}