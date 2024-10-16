static void free_persistent_gnts(struct xen_blkif_ring *ring, struct rb_root *root,
                                 unsigned int num)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct persistent_gnt *persistent_gnt;
	struct rb_node *n;
	int segs_to_unmap = 0;
	struct gntab_unmap_queue_data unmap_data;

	unmap_data.pages = pages;
	unmap_data.unmap_ops = unmap;
	unmap_data.kunmap_ops = NULL;

	foreach_grant_safe(persistent_gnt, n, root, node) {
		BUG_ON(persistent_gnt->handle ==
			BLKBACK_INVALID_HANDLE);
		gnttab_set_unmap_op(&unmap[segs_to_unmap],
			(unsigned long) pfn_to_kaddr(page_to_pfn(
				persistent_gnt->page)),
			GNTMAP_host_map,
			persistent_gnt->handle);

		pages[segs_to_unmap] = persistent_gnt->page;

		if (++segs_to_unmap == BLKIF_MAX_SEGMENTS_PER_REQUEST ||
			!rb_next(&persistent_gnt->node)) {

			unmap_data.count = segs_to_unmap;
			BUG_ON(gnttab_unmap_refs_sync(&unmap_data));

			put_free_pages(ring, pages, segs_to_unmap);
			segs_to_unmap = 0;
		}

		rb_erase(&persistent_gnt->node, root);
		kfree(persistent_gnt);
		num--;
	}
	BUG_ON(num != 0);
}