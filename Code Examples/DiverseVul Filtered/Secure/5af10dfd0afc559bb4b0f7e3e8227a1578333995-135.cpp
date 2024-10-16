static long region_chg(struct resv_map *resv, long f, long t)
{
	struct list_head *head = &resv->regions;
	struct file_region *rg, *nrg = NULL;
	long chg = 0;

retry:
	spin_lock(&resv->lock);
retry_locked:
	resv->adds_in_progress++;

	/*
	 * Check for sufficient descriptors in the cache to accommodate
	 * the number of in progress add operations.
	 */
	if (resv->adds_in_progress > resv->region_cache_count) {
		struct file_region *trg;

		VM_BUG_ON(resv->adds_in_progress - resv->region_cache_count > 1);
		/* Must drop lock to allocate a new descriptor. */
		resv->adds_in_progress--;
		spin_unlock(&resv->lock);

		trg = kmalloc(sizeof(*trg), GFP_KERNEL);
		if (!trg) {
			kfree(nrg);
			return -ENOMEM;
		}

		spin_lock(&resv->lock);
		list_add(&trg->link, &resv->region_cache);
		resv->region_cache_count++;
		goto retry_locked;
	}

	/* Locate the region we are before or in. */
	list_for_each_entry(rg, head, link)
		if (f <= rg->to)
			break;

	/* If we are below the current region then a new region is required.
	 * Subtle, allocate a new region at the position but make it zero
	 * size such that we can guarantee to record the reservation. */
	if (&rg->link == head || t < rg->from) {
		if (!nrg) {
			resv->adds_in_progress--;
			spin_unlock(&resv->lock);
			nrg = kmalloc(sizeof(*nrg), GFP_KERNEL);
			if (!nrg)
				return -ENOMEM;

			nrg->from = f;
			nrg->to   = f;
			INIT_LIST_HEAD(&nrg->link);
			goto retry;
		}

		list_add(&nrg->link, rg->link.prev);
		chg = t - f;
		goto out_nrg;
	}

	/* Round our left edge to the current segment if it encloses us. */
	if (f > rg->from)
		f = rg->from;
	chg = t - f;

	/* Check for and consume any regions we now overlap with. */
	list_for_each_entry(rg, rg->link.prev, link) {
		if (&rg->link == head)
			break;
		if (rg->from > t)
			goto out;

		/* We overlap with this area, if it extends further than
		 * us then we must extend ourselves.  Account for its
		 * existing reservation. */
		if (rg->to > t) {
			chg += rg->to - t;
			t = rg->to;
		}
		chg -= rg->to - rg->from;
	}

out:
	spin_unlock(&resv->lock);
	/*  We already know we raced and no longer need the new region */
	kfree(nrg);
	return chg;
out_nrg:
	spin_unlock(&resv->lock);
	return chg;
}