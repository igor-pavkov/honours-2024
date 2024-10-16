static long region_add(struct resv_map *resv, long f, long t)
{
	struct list_head *head = &resv->regions;
	struct file_region *rg, *nrg, *trg;
	long add = 0;

	spin_lock(&resv->lock);
	/* Locate the region we are either in or before. */
	list_for_each_entry(rg, head, link)
		if (f <= rg->to)
			break;

	/*
	 * If no region exists which can be expanded to include the
	 * specified range, the list must have been modified by an
	 * interleving call to region_del().  Pull a region descriptor
	 * from the cache and use it for this range.
	 */
	if (&rg->link == head || t < rg->from) {
		VM_BUG_ON(resv->region_cache_count <= 0);

		resv->region_cache_count--;
		nrg = list_first_entry(&resv->region_cache, struct file_region,
					link);
		list_del(&nrg->link);

		nrg->from = f;
		nrg->to = t;
		list_add(&nrg->link, rg->link.prev);

		add += t - f;
		goto out_locked;
	}

	/* Round our left edge to the current segment if it encloses us. */
	if (f > rg->from)
		f = rg->from;

	/* Check for and consume any regions we now overlap with. */
	nrg = rg;
	list_for_each_entry_safe(rg, trg, rg->link.prev, link) {
		if (&rg->link == head)
			break;
		if (rg->from > t)
			break;

		/* If this area reaches higher then extend our area to
		 * include it completely.  If this is not the first area
		 * which we intend to reuse, free it. */
		if (rg->to > t)
			t = rg->to;
		if (rg != nrg) {
			/* Decrement return value by the deleted range.
			 * Another range will span this area so that by
			 * end of routine add will be >= zero
			 */
			add -= (rg->to - rg->from);
			list_del(&rg->link);
			kfree(rg);
		}
	}

	add += (nrg->from - f);		/* Added to beginning of region */
	nrg->from = f;
	add += t - nrg->to;		/* Added to end of region */
	nrg->to = t;

out_locked:
	resv->adds_in_progress--;
	spin_unlock(&resv->lock);
	VM_BUG_ON(add < 0);
	return add;
}