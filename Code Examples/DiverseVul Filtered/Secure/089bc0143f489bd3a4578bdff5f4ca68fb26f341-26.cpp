static inline void put_free_pages(struct xen_blkif_ring *ring, struct page **page,
                                  int num)
{
	unsigned long flags;
	int i;

	spin_lock_irqsave(&ring->free_pages_lock, flags);
	for (i = 0; i < num; i++)
		list_add(&page[i]->lru, &ring->free_pages);
	ring->free_pages_num += num;
	spin_unlock_irqrestore(&ring->free_pages_lock, flags);
}