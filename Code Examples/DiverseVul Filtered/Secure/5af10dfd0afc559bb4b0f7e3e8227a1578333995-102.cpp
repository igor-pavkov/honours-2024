static long region_count(struct resv_map *resv, long f, long t)
{
	struct list_head *head = &resv->regions;
	struct file_region *rg;
	long chg = 0;

	spin_lock(&resv->lock);
	/* Locate each segment we overlap with, and count that overlap. */
	list_for_each_entry(rg, head, link) {
		long seg_from;
		long seg_to;

		if (rg->to <= f)
			continue;
		if (rg->from >= t)
			break;

		seg_from = max(rg->from, f);
		seg_to = min(rg->to, t);

		chg += seg_to - seg_from;
	}
	spin_unlock(&resv->lock);

	return chg;
}