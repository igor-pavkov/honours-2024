struct hugepage_subpool *hugepage_new_subpool(struct hstate *h, long max_hpages,
						long min_hpages)
{
	struct hugepage_subpool *spool;

	spool = kzalloc(sizeof(*spool), GFP_KERNEL);
	if (!spool)
		return NULL;

	spin_lock_init(&spool->lock);
	spool->count = 1;
	spool->max_hpages = max_hpages;
	spool->hstate = h;
	spool->min_hpages = min_hpages;

	if (min_hpages != -1 && hugetlb_acct_memory(h, min_hpages)) {
		kfree(spool);
		return NULL;
	}
	spool->rsv_hpages = min_hpages;

	return spool;
}