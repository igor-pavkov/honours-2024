static unsigned int xen_blkbk_unmap_prepare(
	struct xen_blkif_ring *ring,
	struct grant_page **pages,
	unsigned int num,
	struct gnttab_unmap_grant_ref *unmap_ops,
	struct page **unmap_pages)
{
	unsigned int i, invcount = 0;

	for (i = 0; i < num; i++) {
		if (pages[i]->persistent_gnt != NULL) {
			put_persistent_gnt(ring, pages[i]->persistent_gnt);
			continue;
		}
		if (pages[i]->handle == BLKBACK_INVALID_HANDLE)
			continue;
		unmap_pages[invcount] = pages[i]->page;
		gnttab_set_unmap_op(&unmap_ops[invcount], vaddr(pages[i]->page),
				    GNTMAP_host_map, pages[i]->handle);
		pages[i]->handle = BLKBACK_INVALID_HANDLE;
		invcount++;
       }

       return invcount;
}