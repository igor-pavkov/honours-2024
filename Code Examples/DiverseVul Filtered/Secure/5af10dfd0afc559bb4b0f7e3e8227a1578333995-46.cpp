void hugepage_put_subpool(struct hugepage_subpool *spool)
{
	spin_lock(&spool->lock);
	BUG_ON(!spool->count);
	spool->count--;
	unlock_or_release_subpool(spool);
}