int __weak alloc_bootmem_huge_page(struct hstate *h)
{
	struct huge_bootmem_page *m;
	int nr_nodes, node;

	for_each_node_mask_to_alloc(h, nr_nodes, node, &node_states[N_MEMORY]) {
		void *addr;

		addr = memblock_virt_alloc_try_nid_nopanic(
				huge_page_size(h), huge_page_size(h),
				0, BOOTMEM_ALLOC_ACCESSIBLE, node);
		if (addr) {
			/*
			 * Use the beginning of the huge page to store the
			 * huge_bootmem_page struct (until gather_bootmem
			 * puts them into the mem_map).
			 */
			m = addr;
			goto found;
		}
	}
	return 0;

found:
	BUG_ON(!IS_ALIGNED(virt_to_phys(m), huge_page_size(h)));
	/* Put them into a private list first because mem_map is not up yet */
	list_add(&m->list, &huge_boot_pages);
	m->hstate = h;
	return 1;
}