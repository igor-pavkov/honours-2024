struct cipso_v4_doi *cipso_v4_doi_getdef(u32 doi)
{
	struct cipso_v4_doi *doi_def;

	rcu_read_lock();
	doi_def = cipso_v4_doi_search(doi);
	if (!doi_def)
		goto doi_getdef_return;
	if (!refcount_inc_not_zero(&doi_def->refcount))
		doi_def = NULL;

doi_getdef_return:
	rcu_read_unlock();
	return doi_def;
}