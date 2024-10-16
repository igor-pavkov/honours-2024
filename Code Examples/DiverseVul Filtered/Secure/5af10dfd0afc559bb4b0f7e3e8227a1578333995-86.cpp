static void region_abort(struct resv_map *resv, long f, long t)
{
	spin_lock(&resv->lock);
	VM_BUG_ON(!resv->region_cache_count);
	resv->adds_in_progress--;
	spin_unlock(&resv->lock);
}