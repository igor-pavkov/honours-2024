static void account_kernel_stack(struct thread_info *ti, int account)
{
	struct zone *zone = page_zone(virt_to_page(ti));

	mod_zone_page_state(zone, NR_KERNEL_STACK, account);
}