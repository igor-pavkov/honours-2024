static inline void shrink_free_pagepool(struct xen_blkif_ring *ring, int num)
{
	/* Remove requested pages in batches of NUM_BATCH_FREE_PAGES */
	struct page *page[NUM_BATCH_FREE_PAGES];
	unsigned int num_pages = 0;
	unsigned long flags;

	spin_lock_irqsave(&ring->free_pages_lock, flags);
	while (ring->free_pages_num > num) {
		BUG_ON(list_empty(&ring->free_pages));
		page[num_pages] = list_first_entry(&ring->free_pages,
		                                   struct page, lru);
		list_del(&page[num_pages]->lru);
		ring->free_pages_num--;
		if (++num_pages == NUM_BATCH_FREE_PAGES) {
			spin_unlock_irqrestore(&ring->free_pages_lock, flags);
			gnttab_free_pages(num_pages, page);
			spin_lock_irqsave(&ring->free_pages_lock, flags);
			num_pages = 0;
		}
	}
	spin_unlock_irqrestore(&ring->free_pages_lock, flags);
	if (num_pages != 0)
		gnttab_free_pages(num_pages, page);
}