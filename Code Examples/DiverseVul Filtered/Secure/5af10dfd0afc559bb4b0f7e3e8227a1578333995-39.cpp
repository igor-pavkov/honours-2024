static long region_del(struct resv_map *resv, long f, long t)
{
	struct list_head *head = &resv->regions;
	struct file_region *rg, *trg;
	struct file_region *nrg = NULL;
	long del = 0;

retry:
	spin_lock(&resv->lock);
	list_for_each_entry_safe(rg, trg, head, link) {
		/*
		 * Skip regions before the range to be deleted.  file_region
		 * ranges are normally of the form [from, to).  However, there
		 * may be a "placeholder" entry in the map which is of the form
		 * (from, to) with from == to.  Check for placeholder entries
		 * at the beginning of the range to be deleted.
		 */
		if (rg->to <= f && (rg->to != rg->from || rg->to != f))
			continue;

		if (rg->from >= t)
			break;

		if (f > rg->from && t < rg->to) { /* Must split region */
			/*
			 * Check for an entry in the cache before dropping
			 * lock and attempting allocation.
			 */
			if (!nrg &&
			    resv->region_cache_count > resv->adds_in_progress) {
				nrg = list_first_entry(&resv->region_cache,
							struct file_region,
							link);
				list_del(&nrg->link);
				resv->region_cache_count--;
			}

			if (!nrg) {
				spin_unlock(&resv->lock);
				nrg = kmalloc(sizeof(*nrg), GFP_KERNEL);
				if (!nrg)
					return -ENOMEM;
				goto retry;
			}

			del += t - f;

			/* New entry for end of split region */
			nrg->from = t;
			nrg->to = rg->to;
			INIT_LIST_HEAD(&nrg->link);

			/* Original entry is trimmed */
			rg->to = f;

			list_add(&nrg->link, &rg->link);
			nrg = NULL;
			break;
		}

		if (f <= rg->from && t >= rg->to) { /* Remove entire region */
			del += rg->to - rg->from;
			list_del(&rg->link);
			kfree(rg);
			continue;
		}

		if (f <= rg->from) {	/* Trim beginning of region */
			del += t - rg->from;
			rg->from = t;
		} else {		/* Trim end of region */
			del += rg->to - f;
			rg->to = f;
		}
	}

	spin_unlock(&resv->lock);
	kfree(nrg);
	return del;
}